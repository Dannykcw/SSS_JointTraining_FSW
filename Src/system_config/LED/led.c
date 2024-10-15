/*
 * led.c
 *
 *  - Nov 8-9, 2023
 *      Author       : nithinsenthil
 *      Log          : Updated LED GPIO config for OP Rev2
 *
 *  - Apr 29, 2023 (Creation)
 *      Author       : Tim S.
 *      Contributors : nithinsenthil , Raphael
 *      Log          : Led Control functions written
 */

#include "led.h"

/***************************** LED INITIALIZERS ******************************/

void led_init() {

# if OP_REV == 2

	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;

	while (GPIOE->OTYPER == 0xFFFFFFFF);

	// Configure output mode
	GPIOE->MODER &= ~(
		  GPIO_MODER_MODE2_Msk
		| GPIO_MODER_MODE3_Msk
		| GPIO_MODER_MODE4_Msk
		| GPIO_MODER_MODE5_Msk
		| GPIO_MODER_MODE6_Msk);

	GPIOE->MODER |=
		  GPIO_MODER_MODE2_0	// Heartbeat
		| GPIO_MODER_MODE3_0	// D3
		| GPIO_MODER_MODE4_0	// D2
		| GPIO_MODER_MODE5_0	// D1
		| GPIO_MODER_MODE6_0;	// D0

# endif

}

/******************************* LED TOGGLERS ********************************/

void blinky() {
	static int counter = 0;

	if (counter++ > 100) {
		counter = 0;
	}
	if (counter > 90) {
		led_hb(1);
	} else {
		led_hb(0);
	}
}
