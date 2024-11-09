/*
 * timers.h
 */

#ifndef REALOP1_TIMERS_H_
#define REALOP1_TIMERS_H_

#include "stm32l476xx.h"
#include <globals.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Put PWM stuff here:

void pwm_doSomething();

void pwm_init_timer_gpio();
bool pwm_InitTimer(uint32_t ms, float dutyCycle);
void pwm_setDutyCycle(float dutyCycle);

void testerFunction_PWMGeneration();

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

/**
 * Initializes the Heartbeat LED Timer (the Systck). 
 * Configures it to tick every ms.
 *
 * @param run_scheduler If set to true, the IntelliSat Scheduler will be
 *        call by the Systick
 * @returns none
*/
void systick_init();

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

#define ExpLogTimer      				TIM6
#define ExpLogTimer_ClockEnable() 		RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN
#define ExpLogTimer_ClockDisable() 		RCC->APB1ENR1 &= ~RCC_APB1ENR1_TIM6EN

#define logger_expTimerOn()  ExpLogTimer->CR1 |=  TIM_CR1_CEN;
#define logger_expTimerOff() ExpLogTimer->CR1 &= ~TIM_CR1_CEN;

/**
 * Configures the Experiment Logger timer registers to generate
 * an interrupt every 100 ms.
 *
 * @returns Boolean to indicate if the initialization was successful
 */
bool logger_initExpTimer();

/*
 * Binds a function to the log timer's interrupt handler.
 * Essentially lets the user decide which function to run when the log timer
 * interrupts.
 *
 * @param   A pointer to the interrupt handler function. The function must be
 *          void and take no inputs
 * @returns none
 */
void logger_registerLogFunction(void (*func)());

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

#define StartupTimer					TIM5
#define StartupTimer_ClockEnable()		RCC->APB1ENR1 |=  RCC_APB1ENR1_TIM5EN
#define StartupTimer_ClockDisable()		RCC->APB1ENR1 &= ~RCC_APB1ENR1_TIM5EN

/**
 * Configures the Startup Timer registers to generate
 * an interrupt in 30 minutes
 *
 * @returns Boolean to indicate if the initialization was successful
 */
void timer_waitStartupTime();


#endif	// REALOP1_TIMERS_H_
