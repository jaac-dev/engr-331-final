#include "keypad.h"

void keypad_init() {
	// configure rows as outputs
	gpio_configure_out(key_gpio,r1);
	gpio_configure_out(key_gpio,r2);
	gpio_configure_out(key_gpio,r3);
	gpio_configure_out(key_gpio,r4);
	
	// configure columns as inputs --> done in interrupt function 
	
	// set all rows to HIGH
	reset_rows();

}

void reset_rows() {
	gpio_write(key_gpio,r1,true);
	gpio_write(key_gpio,r2,true);
	gpio_write(key_gpio,r3,true);
	gpio_write(key_gpio,r4,true);
}

void keypad_interrupt(gpio_exti_cb_t callback1, gpio_exti_cb_t callback2, gpio_exti_cb_t callback3) {
	gpio_configure_interrupt(key_gpio, c1, EXTI_EDGE_RISING, PUPD_PULL_DOWN, 1, callback1);
	gpio_configure_interrupt(key_gpio, c2, EXTI_EDGE_RISING, PUPD_PULL_DOWN, 1, callback2);
	gpio_configure_interrupt(key_gpio, c3, EXTI_EDGE_RISING, PUPD_PULL_DOWN, 1, callback3);
}

bool keypad_debounce(uint8_t pin) {
	// check states for a number of cycles and return true if stable is consistently LOW
	delay_ms(5);
	if (gpio_read(key_gpio,pin))
		return true;
	else
		return false;
	
}


int keypad_scan(uint8_t col, uint8_t pin) {
	// set the correct column pin based on the active column
	if (col == 1) {
		pin = c1;
	}
	else if (col == 2) {
		pin = c2;
	}
	else {
		pin = c3;
	}
	
	gpio_write(key_gpio,r2,false);
	gpio_write(key_gpio,r3,false);
	gpio_write(key_gpio,r4,false);
	delay_ms(1);
	// check if row 1 contains the button press
	if (gpio_read(key_gpio,pin)) {
		reset_rows();
		return (col);
	}

	gpio_write(key_gpio,r1,false);

	// move onto row 2
	gpio_write(key_gpio,r2,true);
	delay_ms(1);
	// check if row 2 contains the button press
	if (gpio_read(key_gpio,pin)){
		reset_rows();
		return (col + 3);
	}
	gpio_write(key_gpio,r2,false);
	
	// move onto row 3
	gpio_write(key_gpio,r3,true);
	delay_ms(1);
	// check if row 3 contains the button press
	if (gpio_read(key_gpio,pin)){
		reset_rows();
		return (col + 6);
	}
	else {	// check if row 4 contains the button press (only option left)
		reset_rows();
		if (col == 1)
			return (10);
		else if (col == 2)
			return (11);
		else
			return (12);
	}
}