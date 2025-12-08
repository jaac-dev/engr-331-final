#pragma once

#include <stdint.h>
#include <stm32f407xx.h>

void systick_init();

uint32_t systick_get();