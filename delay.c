#include "delay.h"

#include <stm32f407xx.h>

void delay_init() {
	// Enable the timer for TIM6.
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
}

void delay_ms(uint32_t ms) {
	for (uint32_t i = 0; i < ms; i++) {
		TIM6->CR1 &= ~TIM_CR1_CEN;
		TIM6->PSC = 83;
		TIM6->ARR = 999;
		TIM6->CNT = 0;
		TIM6->CR1 |= TIM_CR1_CEN;
		while (!(TIM6->SR & TIM_SR_UIF));
		TIM6->SR = 0;
		TIM6->CR1 &= ~TIM_CR1_CEN;
	}
}

void delay_us(uint32_t us) {
	for (uint32_t i = 0; i < us; i++) {
		TIM6->CR1 &= ~TIM_CR1_CEN;
		TIM6->PSC = 15;
		TIM6->ARR = 0;
		TIM6->CNT = 0;
		TIM6->SR = 0;
		TIM6->CR1 |= TIM_CR1_CEN;
		while (!(TIM6->SR & TIM_SR_UIF));
		TIM6->SR = 0;
		TIM6->CR1 &= ~TIM_CR1_CEN;
	}
}