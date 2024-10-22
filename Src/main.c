#include <print_scan.h>
#include "platform_init.h"

#include <TestDefinition.h>

void my_main();

int main() {
    init_init();
    init_platform();

    // Some adhoc configuration of gpio
    uint32_t gpioa_moder = GPIOA->MODER;

	gpioa_moder |=  (1 << 30);
	gpioa_moder &= ~(1 << 31);

	GPIOA->MODER = gpioa_moder;

    my_main();
}


void my_main() {

	bool pin_status = 0;
	uint32_t delay = 4000000;
	while (1) {
		// Printing using printf like format
		printMsg("Hello There!\r\n");
//		printMsg("delay: %ul", delay);


		// We can toggle some pins
		gpio_set(GPIOA, 15, pin_status);
		led_d0(pin_status);
		pin_status = !pin_status;
		nop(delay);
	}
}
