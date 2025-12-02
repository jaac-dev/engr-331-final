#pragma once

#include <stm32f407xx.h>

#include <stdint.h>
#include <stdbool.h>

///
/// ADC-mapped Pins Reference
///
/// Port A: pins 0-7 map to channels 0-7.
/// Port B: pins 0-1 map to channels 8-9.
/// Port C: pins 0-5 map to channels 10-15.
///



/// Defines the pull-up/pull-down resistor type for input pin configuration.
typedef enum {
	PUPD_PULL_UP = 0,
	PUPD_PULL_DOWN = 1,
	PUPD_NONE = 2,
} gpio_pupd_t;

typedef enum {
	OT_PUSH_PULL = 0,
	OT_OPEN_DRAIN = 1,
} gpio_ot_t;

/// Defines the external interrupt edge trigger.
typedef enum {
	EXTI_EDGE_RISING,
  EXTI_EDGE_FALLING,
  EXTI_EDGE_BOTH
} gpio_exti_edge_t;

typedef void (*gpio_exti_cb_t)(void);

/// Enable the GPIO pin in the given block as an output.
void gpio_configure_out(GPIO_TypeDef *block, uint8_t pin);

/// Enable the GPIO pin in the given block as an input.
void gpio_configure_in(GPIO_TypeDef *block, uint8_t pin, gpio_pupd_t pupd);

/// Enable the GPIO pin in the given block as an analog input.
/// NOTE: This only works with ADC1-mapped pins. 
void gpio_configure_in_analog(GPIO_TypeDef *block, uint8_t pin);

/// Configures the GPIO pin in the given block as an external interrupt.
void gpio_configure_interrupt(
	GPIO_TypeDef *block, 
	uint8_t pin, 
	gpio_exti_edge_t edge,
	gpio_pupd_t pupd,
	uint8_t priority,
	gpio_exti_cb_t callback
);

/// Writes a value to the given GPIO pin on the given block.
void gpio_write(GPIO_TypeDef *block, uint8_t pin, bool value);

/// Reads a value from the given GPIO pin on the given block.
bool gpio_read(GPIO_TypeDef *block, uint8_t pin);

/// Reads an analog value from the given GPIO pin the given block.
/// NOTE: This only works with ADC1-mapped pins.
uint16_t gpio_read_analog(GPIO_TypeDef *block, uint8_t pin);
