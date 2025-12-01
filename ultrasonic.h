#pragma once

#include <stm32f407xx.h>

typedef struct {
	GPIO_TypeDef *block;
	uint8_t pin;
} ultrasonic_pin_t;

typedef struct {
	ultrasonic_pin_t echo;
	ultrasonic_pin_t trig;
	
	TIM_TypeDef *timer;
	uint8_t timer_channel;
} ultrasonic_t;

void ultrasonic_init(ultrasonic_t *self);

void ultrasonic_interrupt(ultrasonic_t *self);