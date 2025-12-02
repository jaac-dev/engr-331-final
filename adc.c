#include "adc.h"

void adc_init() {
	// Enable the ADC1 clock.
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

	// PCLK2 / 4 for the ADC prescalar.
	ADC->CCR &= ~(3 << ADC_CCR_ADCPRE_Pos);
	ADC->CCR |= (1 << ADC_CCR_ADCPRE_Pos);
	
	// Power on ADC1.
	ADC1->CR2 |= ADC_CR2_ADON;
}

uint16_t adc_read(uint8_t channel) {
	// Configure the channel to sample for 144 cycles.
	if (channel <= 9) {
		ADC1->SMPR2 &= ~(7 << (channel * 3));
		ADC1->SMPR2 |= (6 << (channel * 3));
	} else if (channel <= 18) {
		ADC1->SMPR1 &= ~(7 << ((channel - 10) * 3));
		ADC1->SMPR1 |= (6 << ((channel - 10) * 3));
	} else {
		return 0; // Invalid channel.
	}
	
	// Clear out the sequence length, we only will perform one conversion.
	ADC1->SQR1 &= ~(0xF << ADC_SQR1_L_Pos);
	
	// Write the given channel into conversion 1.
	ADC1->SQR3 &= ~0x1F;
	ADC1->SQR3 |= channel & 0x1F;
	
	// Start the conversion.
	ADC1->CR2 |= ADC_CR2_SWSTART;

	// Wait for the EOC bit to be set.
	while ((ADC1->SR & ADC_SR_EOC) == 0);
	
	// Return the converted value.
	return ADC1->DR & 0x0FFF;
}