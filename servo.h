#pragma once

#include <stm32f407xx.h>

typedef struct {
	// The GPIO block of the PWM pin.
	GPIO_TypeDef *pwm_block;
	
	// The PWM pin index.
	uint8_t pwm_pin;
	
	// The alternate function to configure, mapping the pin to the given timer.
	uint8_t pwm_af;

	// The timer to use.
	TIM_TypeDef *timer;
} servo_t;

void servo_init(servo_t *self, uint32_t init_pw);

void servo_set_pulse_width(servo_t *self, uint32_t pw);