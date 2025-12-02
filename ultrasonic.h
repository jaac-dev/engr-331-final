#pragma once

#include <stm32f407xx.h>

#include <stdbool.h>

typedef struct {
	GPIO_TypeDef *block;
	uint8_t pin;
} ultrasonic_pin_t;

typedef struct {
	// The pin to use for ECHO.
	ultrasonic_pin_t echo; 
	
	// The pin to use for TRIG.
	ultrasonic_pin_t trig; 
	
	// The alternate function to use for ECHO.
	// This needs to map to the specified timer and channel.
	uint8_t echo_af;
	
	// The timer to use.
	TIM_TypeDef *timer;
	
	// The channel on the timer to use.
	uint8_t timer_channel;
	
	// The two capture values for finding the pulse width.
	volatile uint32_t v1; 
	volatile uint32_t v2;
	
	// False when reading rising edge, and true for falling.
	volatile bool edge;
	
	// Set to true when a measurement is done.
	volatile bool done;
} ultrasonic_t;

void ultrasonic_init(ultrasonic_t *self);

void ultrasonic_interrupt(ultrasonic_t *self);

float ultrasonic_read(ultrasonic_t *self);