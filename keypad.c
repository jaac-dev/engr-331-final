#include "keypad.h"

const char KEYPAD_LAYOUT[KEYPAD_ROWS][KEYPAD_COLS] = {
	{ '1', '2', '3' },
	{ '4', '5', '6' },
	{ '7', '8', '9' },
	{ '*', '0', '#' },
};

static void keypad_rows_high(keypad_t *self) {
	for (uint8_t i = 0; i < KEYPAD_ROWS; i++)
		gpio_write(self->row_io[i].block, self->row_io[i].pin, true);
}
/*
void keypad_init(keypad_t *self) {
	// PD15 - C2
	// PD14 - R1
	// PD13 - C1
	// PD12 - R4
	// PD11 - C3
	// PD9 - R3
	// PD10 - R2

	for (uint8_t i = 0; i < KEYPAD_ROWS; i++)
		gpio_configure_in(self->row_io[i].block, self->row_io[i].pin, PUPD_PULL_UP);
	
	for (uint8_t i = 0; i < KEYPAD_COLS; i++)
		gpio_configure_out(self->col_io[i].block, self->col_io[i].pin);
	
	for (uint8_t r = 0; r < KEYPAD_ROWS; r++)
		for (uint8_t c = 0; c < KEYPAD_COLS; c++) {
			self->state[r][c] = 0;
			self->debounce[r][c] = 0;
		}

	keypad_rows_high(self);
}

void keypad_tick(keypad_t *self) {

}
*/

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


int keypad_scan(uint8_t col) {
	int pin;
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
			return (0);
		else
			return (11);
	}
}