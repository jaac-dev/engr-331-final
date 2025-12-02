#include "systick.h"

#include <stm32f407xx.h>

volatile uint32_t s_ticks = 0;

void systick_init() {
	// Tick every 1ms.
	SysTick->LOAD = SystemCoreClock / 1000 - 1;

	// Clear the value.
	SysTick->VAL = 0;

	// Configure the timer.
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | // System core clock.
								  SysTick_CTRL_TICKINT_Msk | // Interrupts.
						      SysTick_CTRL_ENABLE_Msk; // Enable.
}

uint32_t systick_get() {
	return s_ticks;
}

void SysTick_Handler() {
	s_ticks++;
}