#pragma once

#include "gpio.h"

#define KEYPAD_ROWS 4
#define KEYPAD_COLS 3

typedef struct {
	GPIO_TypeDef *block;
	uint8_t pin;
} keypad_io_t;

typedef struct {
	keypad_io_t row_io[KEYPAD_ROWS];
	keypad_io_t col_io[KEYPAD_COLS];

	uint8_t state[KEYPAD_ROWS][KEYPAD_COLS];
	uint8_t debounce[KEYPAD_ROWS][KEYPAD_COLS];
} keypad_t;

void keypad_init(keypad_t *self);