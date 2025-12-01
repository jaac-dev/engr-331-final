#include "ultrasonic.h"

void ultrasonic_init(ultrasonic_t *self) {
	// Configure the timer.
	TIM_TypeDef *timer = self->timer;
	uint8_t timer_channel = self->timer_channel;
	
	// Disable the timer.
	timer->CR1 &= ~TIM_CR1_CEN;
	
	// 1 micro-second ticks.
	timer->PSC = 83;
	timer->ARR = 0xFFFFFFFF;
	
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
			timer->CCMR1 &= ~TIM_CCMR2_CC3S;
			timer->CCMR1 |= TIM_CCMR2_CC3S_0;
		
			timer->CCER &= ~TIM_CCER_CC3P;
			timer->CCER |= TIM_CCER_CC3E;
		
			timer->DIER |= TIM_DIER_CC3IE;
			break;
		
		case 4: // Channel 4
			timer->CCMR1 &= ~TIM_CCMR2_CC4S;
			timer->CCMR1 |= TIM_CCMR2_CC4S_0;
		
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