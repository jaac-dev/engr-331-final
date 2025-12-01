#pragma once

#include <stdint.h>

///
/// Note: This system uses the TIM6 peripheral.
///

//=======================================//
//                 DELAYS                //
//=======================================//

/// Initializes the delay system.
void delay_init();

/// Delays the given amount of milliseconds.
void delay_ms(uint32_t ms);

/// Delays the given amount of microseconds.
void delay_us(uint32_t us);