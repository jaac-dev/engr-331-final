#include "delay.h"

void delay_init() {
	// Enable the timer for TIM6.
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	
	TIM6->PSC = 84 - 1;
	TIM6->ARR = 0xFFFF;
	TIM6->CNT = 0;
	TIM6->CR1 = TIM_CR1_CEN;
}

void delay_ms(uint32_t ms) {
	while (ms--)
		delay_us(1000);
}

void delay_us_inner(uint32_t us) {
	uint16_t start = TIM6->CNT;
	while ((uint16_t)(TIM6->CNT - start) < us) {}
}

void delay_us(uint32_t us) {
	while (us) {
		uint16_t chunk = (us > 0xFFFF) ? 0xFFFF : (uint16_t)us;
		delay_us(chunk);
		us -= chunk;
	}
}

/// Function for initializing the high-speed clock
void hs_clock_init() {
	// Enable the high speed oscillator.
	RCC->CR |= RCC_CR_HSEON;
	while ((RCC->CR & RCC_CR_HSERDY) == 0) {}
		
	// Set the power scale.
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR |= PWR_CR_VOS;

	// Configure the flash latency.
	FLASH->ACR =
		FLASH_ACR_ICEN |
		FLASH_ACR_DCEN |
		FLASH_ACR_PRFTEN |
    FLASH_ACR_LATENCY_5WS;
	while ((FLASH->ACR & FLASH_ACR_LATENCY) != FLASH_ACR_LATENCY_5WS) {}

	// Configure the prescalars.
	RCC->CFGR &= ~(RCC_CFGR_HPRE  |
	  RCC_CFGR_PPRE1 |
	  RCC_CFGR_PPRE2);
		
	RCC->CFGR |=
		RCC_CFGR_HPRE_DIV1  |  // AHB  = 168 MHz
		RCC_CFGR_PPRE1_DIV4 |  // APB1 = 42 MHz
		RCC_CFGR_PPRE2_DIV2;   // APB2 = 84 MHz

	// Configure the PLL.
	RCC->CR &= ~RCC_CR_PLLON;
  while (RCC->CR & RCC_CR_PLLRDY) {}

	RCC->PLLCFGR = 0;

	// PLLM = 8
	RCC->PLLCFGR |= 8U << 0;
	// PLLN = 336
	RCC->PLLCFGR |= 336U << 6;
	// PLLP = 2  -> PLLP bits = 00
	// (already 0 from clearing, so nothing to do)
	// PLL source = HSE
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE;
	// PLLQ = 7  -> 48 MHz for USB, etc.
	RCC->PLLCFGR |= 7U << 24;

	// 6. Enable PLL and wait
	RCC->CR |= RCC_CR_PLLON;
	while ((RCC->CR & RCC_CR_PLLRDY) == 0) {}

	// 7. Switch SYSCLK to PLL
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL;   // select PLL as system clock

	// Wait until PLL is used as system clock
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}
	
  // Update the CMSIS core clock.
	SystemCoreClock = 168000000U;
}