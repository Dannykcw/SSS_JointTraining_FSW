// Put implementations of PWM related functions in this file

#include "timers.h"
#include <globals.h>

static void pwm_gpio_init() {

    // Enable GPIO A
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    // ??? Used elsewere, idk if required
    while (GPIOA->OTYPER == 0xFFFFFFFF);

    // Put pin A15 into AF mode (mode 10)
    GPIOA->MODER &= ~GPIO_MODER_MODE15;
    GPIOA->MODER |= GPIO_MODER_MODE15_1;

    // Set A15 to AF1 (0001 - TIM2 Ch1)
    GPIOA->AFR[1] &= ~GPIO_AFRH_AFSEL15_Msk;
    GPIOA->AFR[1] |= GPIO_AFRH_AFSEL15_0;
}

bool pwm_initTimer(uint32_t period) {

    // Some Notes:
    // We have an input clock of 5MHz (APB1, PCLK1 runs at 10MHz ?)
    // Lets aim for the following specs:
    // - 1us tick rate (Clock of 1MHz) - divide by
    // - 20ms total loop, 50Hz (ARR of 20000)
    // - Need duty cycle (and thus pulse wdiths) btwn 1100us and 1900us
    //   - Default to 1500us 


    pwm_gpio_init();
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;

    // Reset Slave Mode Disabled (Should be all 0 by default)
    // TIM2->SMCR &= ~TIM_SMCR_SMS;

    // Calculate Time Base (Section 2.2 of AN4013)
    // Update_event = TIM_CLK / ( (PSC + 1) * ( ARR  + 1) * (RCR + 1))
    //         50Hz =   5MHz  / ( ( 9  + 1) * (19999 + 1) * ( 0  + 1))
    // PSC = 9
    // ARR = 19999
    TIM2->PSC = 9;


    // 1a. Select the output mode by writing CCS bits in CCMRx register
    //     configure CC1 channel as output
    //     TIM2->CCMR1 CC1S = 00
    TIM2->CCMR1 &= ~TIM_CCMR1_CC1S;

    // 1b. Select the polarity by writing the CCxP bit in CCER register
    // TIM2->CCER CC1P = 0
    TIM2->CCER &= ~TIM_CCER_CC1P;

    // 2. Select the PWM mode (PWM1 or PWM2) by writing OCxM bits in CCMRx register
    //    Set PWM Mode to PWM mode 1
    //    TIM2->CCMR1[16, 6:4](OC2M[3,2:0]) = 0110
    TIM2->CCMR1 &= ~TIM_CCMR1_OC1M;
    TIM2->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;


    // 3. Program the period and the duty cycle respectively in ARR and CCRx registers
    //    Set period (ARR) to 20000 - 1
    TIM2->ARR &= ~TIM_ARR_ARR;
    TIM2->ARR |= (period - 1);

    
    // 3. Program duty cycle in CCRx register
    // Default to 15% duty cycle (1500us), which is 1500
    TIM2->CCR1 = 1500;


    // 4. Set the preload bit in CCMRx register and the ARPE bit in the CR1 register
    // 4a. Set preload bit in CCMRx
    // TIM2->CCMR1[11] (OC2PE) = 1
    TIM2->CCMR1 |= TIM_CCMR1_OC1PE;


    // 4b. Set auto-reload preload enable bit in CR1
    // TIM2->CR1[7] (ARPE) = 1
    TIM2->CR1 |= TIM_CR1_ARPE;


    // 5. Set Counting Mode
    // Use CMS bits of TIM2_CR1
    // We'll use PWM edge-aligned (00)
    TIM2->CR1 &= ~TIM_CR1_CMS;


    // 6. Enable the capture compare
    TIM2->CCER |= TIM_CCER_CC1E;


    // Trigger Register Update
    TIM2->EGR |= TIM_EGR_UG;

    // 7. Enable the counter - use PWM_TIMER_ON()
//    TIM2->CR1 |= TIM_CR1_CEN;
}

void pwm_setDutyCycle(float duty_cycle) {
    uint32_t period = TIM2->ARR;
    TIM2->CCR1 = (uint32_t) (duty_cycle/100 * period);
}
