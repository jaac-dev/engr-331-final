#include "ultrasonic.h"

static inline uint32_t read_ccr(TIM_TypeDef *tim, uint8_t ch) {
  switch (ch) {
    case 1: return tim->CCR1;
    case 2: return tim->CCR2;
    case 3: return tim->CCR3;
    case 4: return tim->CCR4;
    default: return 0;
  }
}

void ultrasonic_init(ultrasonic_t *self) {
	// Configure the pins.
	ultrasonic_pin_t *trig = &self->trig;
	ultrasonic_pin_t *echo = &self->echo;
	
	gpio_configure_out(trig->block, trig->pin);
	trig->block->OSPEEDR &= ~(3 << (2 * trig->pin));
	trig->block->OSPEEDR |= (3 << (2 * trig->pin)); // High-speed output.
	trig->block->PUPDR &= ~(3 << (2 * trig->pin)); // No pull resistors.
	
	echo->block->MODER &= ~(3 << (2 * echo->pin));
	echo->block->MODER |= (2 << (2 * echo->pin)); // Alternate function mode.
	echo->block->PUPDR &= ~(3 << (2 * echo->pin));
	echo->block->PUPDR |=  (2U << (2 * echo->pin));
	
	uint8_t afr_idx   = echo->pin / 8;
	uint8_t afr_shift = (echo->pin % 8) * 4;
	
	echo->block->AFR[afr_idx] &= ~(0xF << afr_shift);
	echo->block->AFR[afr_idx] |= ((self->echo_af & 0xF) << afr_shift); // Set the alternate function.
	
	// Configure the timer.
	TIM_TypeDef *timer = self->timer;
	uint8_t timer_channel = self->timer_channel;
	
	// Disable the timer.
	timer->CR1 &= ~TIM_CR1_CEN;
	
	// 1 micro-second ticks.
	timer->PSC = 83;
	timer->ARR = 0xFFFFFFFF;
	
	// Update the timer.
  timer->EGR |= TIM_EGR_UG;
	
	// Map the given channel to TI1, enable rising edge trigger.
	switch (timer_channel) {
		case 1: // Channel 1
			timer->CCMR1 &= ~TIM_CCMR1_CC1S;
			timer->CCMR1 |= TIM_CCMR1_CC1S_0;
		
			timer->CCER &= ~TIM_CCER_CC1P;
			timer->CCER |= TIM_CCER_CC1E;
		
			timer->DIER |= TIM_DIER_CC1IE;
			break;
		
		case 2: // Channel 2
			timer->CCMR1 &= ~TIM_CCMR1_CC2S;
			timer->CCMR1 |= TIM_CCMR1_CC2S_0;
			
			timer->CCER &= ~TIM_CCER_CC2P;
			timer->CCER |= TIM_CCER_CC2E;
			
			timer->DIER |= TIM_DIER_CC2IE;
			break;
		
		case 3: // Channel 3
			timer->CCMR2 &= ~TIM_CCMR2_CC3S;
			timer->CCMR2 |= TIM_CCMR2_CC3S_0;
		
			timer->CCER &= ~TIM_CCER_CC3P;
			timer->CCER |= TIM_CCER_CC3E;
		
			timer->DIER |= TIM_DIER_CC3IE;
			break;
		
		case 4: // Channel 4
			timer->CCMR2 &= ~TIM_CCMR2_CC4S;
			timer->CCMR2 |= TIM_CCMR2_CC4S_0;
		
			timer->CCER &= ~TIM_CCER_CC4P;
			timer->CCER |= TIM_CCER_CC4E;
		
			timer->DIER |= TIM_DIER_CC4IE;
			break;
		
		default: // ????
			break;
	}
	
	// Enable the timer.
	timer->CR1 |= TIM_CR1_CEN;
	
	// Enable the interrupt.
	if (timer == TIM2)
		NVIC_EnableIRQ(TIM2_IRQn);
	else if (timer == TIM3)
		NVIC_EnableIRQ(TIM3_IRQn);
	else if (timer == TIM4)
		NVIC_EnableIRQ(TIM4_IRQn);
	else if (timer == TIM5)
		NVIC_EnableIRQ(TIM5_IRQn);
}

void ultrasonic_interrupt(ultrasonic_t *self) {
	TIM_TypeDef *timer = self->timer;
	timer->SR = 0;
	
	if (!self->active)
		return;
	
	uint32_t value = read_ccr(timer, self->timer_channel);
	
	if (!self->edge) {
		self->v1 = value;
		
		// Switch to falling edge.
		self->edge = true;
		timer->CCER |= (0b0010 << (4 * (self->timer_channel - 1)));
	} else {
		self->v2 = value;
		
		// Switch to rising edge.
		self->edge = false;
		timer->CCER &= ~(0b0010 << (4 * (self->timer_channel - 1)));
		
		self->pulse_width = self->v2 >= self->v1 ? 
			self->v2 - self->v1 : (0xFFFFFFFF - self->v1) + self->v2 + 1;
		
		// Signal that we're done reading.
		self->done = true;
		self->active = false;
	}
}

float ultrasonic_read(ultrasonic_t *self) {
	self->timer->CNT = 0;
	
	self->pulse_width = 0;
	self->v1 = 0;
	self->v2 = 0;
	
	self->edge = false;
	self->done = false;
	
	self->active = true;
	
	gpio_write(self->trig.block, self->trig.pin, true);
	delay_us(10);
	gpio_write(self->trig.block, self->trig.pin, false);
	
	uint32_t start_ts = systick_get();
	while (!self->done)
		if (systick_get() - start_ts > 60)
			return -1.0f;
		
	return self->pulse_width / 58.0f;
}