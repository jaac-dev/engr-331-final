#include "servo.h"
#include "gpio.h"
#include "timer.h"

#define SERVO_PW_MIN 600
#define SERVO_PW_MAX 2400

void servo_init(servo_t *self, uint32_t init_pw) {
	gpio_configure_alternate(self->pwm_block, self->pwm_pin, self->pwm_af);
	
	TIM_TypeDef *timer = self->timer;
	
	// Disable the timer.
	timer->CR1 &= ~TIM_CR1_CEN;
	
	// Enable auto reload preload.
	timer->CR1 |= TIM_CR1_ARPE;
	
	// Enable update interrupts.
	timer->DIER |= TIM_DIER_UIE;
	
	// Set the prescalar and autoreload.
	timer->PSC = 83;
	timer->ARR = 19999;
	
	timer->CCMR1 &= ~TIM_CCMR1_OC1M;
	timer->CCMR1 |=  (6 << TIM_CCMR1_OC1M_Pos);
	timer->CCMR1 |= TIM_CCMR1_OC1PE;
	
	// Set the initial pulse width.
	timer->CCR1 = init_pw;

	// Enable output for the channel.
	timer->CCER |= TIM_CCER_CC1E;
	
	// Update the ARR and PSC.
	timer->EGR |= TIM_EGR_UG;
	
	timer->CR1 |= TIM_CR1_CEN;
}

void servo_set_pulse_width(servo_t *self, uint32_t pw) {
	if (pw < SERVO_PW_MIN)
		pw = SERVO_PW_MIN;
	else if (pw > SERVO_PW_MAX)
		pw = SERVO_PW_MAX;
	
	self->timer->CCR1 = pw;
}