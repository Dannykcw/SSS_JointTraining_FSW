// Put implementations of PWM related functions in this file

#include "timers.h"
#include <globals.h>
#include <math.h>
#include <print_scan.h>

void pwm_doSomething() {
    printMsg("Hello\r\n");
}
/// @brief Initiate the Debug GPIO pin PA15 for ESC-PWM
void pwm_init_timer_gpio() {
    // 	* 0x11
    // 	* 0x11
    // 	* ~ is negation
    // ------------------- //
    // * Enable the gpio (done with one of the RCC AHBENR registers)
    // * RM0351 Section 8.5.1
    // Set the GPIOAEN to enable IO port A clock (binary 1)
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN; // Set bit 0
    // ------------------- //
    // * Set to alternate function mode
    // * RM0351 Section 8.5.1
    // Clear the bits for GPIOA pin 15
    GPIOA->MODER &= ~GPIO_MODER_MODE15; // Clear the 2 bits corresponding to pin 15

    // Set GPIOA pin 15 to Alternate Function mode (binary 10)
    GPIOA->MODER |= GPIO_MODER_MODE15_1; // Set bit 31
    // GPIOA->MODER |= GPIO_MODER_MODE15_0; // Set bit 30
    // ------------------- //
    // Set GPIOA pin 8-15 to PA15 mode (binary 1111)
    GPIOA->AFR[1] &= ~GPIO_AFRH_AFSEL15_Msk;
    GPIOA->AFR[1] |= GPIO_AFRH_AFSEL15_0;
}

/**
 * All reference material:
 * EGR: RM0351 Section 31.3.9, 31.4.6
 * CCMR1: AN4013 Section 2.5, RM0351 Section 31.4.8
 * CHER: RM0351 Section 31.4.11
 * PSC: RM0351 Section 31.4.6
 * ARR: RM0351 Section 31.4.15
 * CR1: RM0351 Section 31.4.1
 * CCER: RM0351 Section 31.4.11
 */
/// @brief Initialize the Pwm timer. For more detail on dutyCycle setting, see pwm_setDutyCycle
/// @param ms period of the wave form in microseconds (μs)
/// @return
bool pwm_InitTimer(uint32_t ms, float dutyCycle) {
    // Enable Timer
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
    // * Initialize all registers
    // Set UG bit to 1 for reinitialization
    TIM2->EGR |= TIM_EGR_UG; // Set bit 0
    // ------------------- //
    // * Configure output pin
    // Set to output mode
    TIM2->CCMR1 &= ~(TIM_CCMR1_CC1S); // Set bit 1:0
    // ------------------- //
    // * Select polarity
    TIM2->CCER &= TIM_CCER_CC1P;
    // ------------------- //
    // * Initialize prescaler
    TIM2->PSC = 9;
    // ------------------- //
    // * Select the PWM mode (PWM1 or PWM2)
    // * by writing OCxM bits in CCMRx register.
    // Set PWM mode 1
    TIM2->CCMR1 &= ~TIM_CCMR1_OC1M; // Set bit 3-6
    TIM2->CCMR1 |= TIM_CCMR1_OC1M_1; // Set bit 5
    TIM2->CCMR1 |= TIM_CCMR1_OC1M_2; // Set bit 6
    // ------------------- //
    // * Program the period
    TIM2->ARR &= ~TIM_ARR_ARR;
    TIM2->ARR |= (uint32_t) (ms - 1); // Write to entire register
    // ------------------- //
    // * Set the dutycycle
    pwm_setDutyCycle(dutyCycle);
    // ------------------- //
    // * Set the preload bit
    TIM2->CCMR1 |= TIM_CCMR1_OC1PE; // Set Bit 3
    // ------------------- //
    // * Set ARPE bit for preload
    TIM2->CR1 |= TIM_CR1_ARPE;
    // ------------------- //
    // set edge align mode
    // TIM2->CR1 &= ~TIM_CR1_CMS;
    // ------------------- //
    // * Enable capture/compare
    TIM2->CCER |= TIM_CCER_CC1E; // Set bit 0

    // RCC->APB1ENR1 |= RCC_APB1SMENR1_TIM2SMEN;

    return 1;
}

/// @brief Set the PWM DutyCycle.
/// Should be in range [0, 100] .
/// But for our case, any value not in range [10,20] values greater or lesser then is forced
/// into the [10,20] range range
/// @param dutyCycle value for pwm dutyCycle as a percentage, e.g., 19.5, -50, etc
void pwm_setDutyCycle(float dutyCycle) {
    // Convert to percentage
    dutyCycle /= 100;
    // Not mapping it any more
    // map [-1, 1] to [.1, .2]
    // https://math.stackexchange.com/questions/914823/shift-numbers-into-a-different-range
    // dutyCycle = dutyCycle / .20 + 0.15;
    if (dutyCycle > 0.20) {
        dutyCycle = 0.20;
    } else if (dutyCycle < 0.1) {
        dutyCycle = 0.1;
    }
    printMsg("DutyCyle: %f\r\n", dutyCycle);
    // DutyCycle = CCR/ARR
    // https://deepbluembedded.com/stm32-pwm-example-timer-pwm-mode-tutorial/
    float pulseWidth = dutyCycle * TIM2->ARR;
    printMsg("pulseWidth: %f\r\n", pulseWidth);
    /**
     * If channel CC1 is configured as output:
     * CCR1 is the value to be loaded
     *  in the actual capture/compare 1 register
     *  (preload value).
     * * RM0351 Section 31.4.16
     */
    TIM2->CCR1 = pulseWidth;
}
