#include "adc.h"

#include <stm32f407xx.h>

void adc_init() {
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

	// Common ADC Config.
	ADC->CCR = 0;
	ADC->CCR |= (1U << 16); // 1/4 prescalar.
	
	// ADC 1 Config.
	ADC1->CR1 = 0;
	ADC1->CR1 |= ADC_CR1_SCAN;
	
  ADC1->CR2 = 0;
	ADC1->CR2 |= ADC_CR2_CONT;
	ADC1->CR2 |= ADC_CR2_DMA;
	ADC1->CR2 |= ADC_CR2_DDS;
	
	// Set the sample rate for the 6 channels.
	uint32_t s = 0b010;
	uint32_t sr = 0;
	for (int i = 0; i < 6; i++)
		sr |= (s << (i * 3));
	
	ADC1->SMPR2 = sr;
}