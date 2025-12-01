#pragma once

#include "delay.h"
#include "gpio.h"

#define KEYPAD_ROWS 4
#define KEYPAD_COLS 3

// GPIOE pins 0-6
#define key_gpio GPIOE
#define c2 2
#define r1 4
#define c1 1
#define r4 0
#define c3 3
#define r3 6
#define r2 5

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

//void keypad_init(keypad_t *self);

/// initialize all input and output pins needed for keypad
void keypad_init();

/// set up interrupt service routine for keypad input pins (actual handler will be located in main)
void keypad_interrupt(gpio_exti_cb_t callback1,	gpio_exti_cb_t callback2,	gpio_exti_cb_t callback3);

/// scan keypad and return the respective value ('*' = -1 and '#' = -2 for now)
int keypad_scan(uint8_t col);

/// resets all rows back to outputting HIGH
void reset_rows();