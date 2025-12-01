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