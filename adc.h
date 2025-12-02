#pragma once

#include <stm32f407xx.h>

void adc_init();

uint16_t adc_read(uint8_t channel);