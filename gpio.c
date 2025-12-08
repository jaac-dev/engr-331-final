#include "gpio.h"
#include "adc.h"

// External Interrupt Callbacks.
static gpio_exti_cb_t s_exti_callbacks[16] = {0};

static inline void gpio_enable_clock(GPIO_TypeDef *block) {
	if (block == GPIOA)
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	else if (block == GPIOB)
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	else if (block == GPIOC)
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	else if (block == GPIOD)
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
	else if (block == GPIOE)
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
}

static inline uint32_t gpio_block_index(GPIO_TypeDef *block) {
  if (block == GPIOA) return 0;
  if (block == GPIOB) return 1;
  if (block == GPIOC) return 2;
  if (block == GPIOD) return 3;
	if (block == GPIOE) return 4;
  return 0;
}

static inline IRQn_Type gpio_exti_line_to_irq(uint8_t line) {
  if (line <= 4) {
    static const IRQn_Type map[5] = {
      EXTI0_IRQn, EXTI1_IRQn, EXTI2_IRQn, EXTI3_IRQn, EXTI4_IRQn
    };
    return map[line];
  } else if (line <= 9) {
    return EXTI9_5_IRQn;
  } else {
    return EXTI15_10_IRQn;
  }
}

/// Maps a block and pin to it's analog channel for ADC1. Returns -1 if no mapping exists.
static inline int8_t gpio_analog_channel(GPIO_TypeDef *block, uint8_t pin) {
	if (block == GPIOA) {
		switch (pin) {
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
				return pin;
			default:
				return -1;
		}
	}
	
	if (block == GPIOB) {
		switch (pin) {
			case 0:
			case 1:
				return pin + 8;
			default:
				return -1;
		}
	}
	
	if (block == GPIOC) {
		switch (pin) {
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				return pin + 10;
			default:
				return -1;
		}
	}
	
	return -1;
}

void gpio_configure_out(GPIO_TypeDef *block, uint8_t pin) {
	gpio_enable_clock(block);
	
	block->MODER = (block->MODER & ~(0b11 << pin * 2)) | (0b01 << pin * 2); // General purpose output.
	block->OTYPER &= ~(0b1 << pin); // Push-pull.
}

void gpio_configure_in(GPIO_TypeDef *block, uint8_t pin, gpio_pupd_t pupd) {
	gpio_enable_clock(block);
	
	uint32_t pos = pin * 2;
  block->MODER &= ~(3 << pos);
  block->PUPDR &= ~(3 << pos);
	if (pupd == PUPD_PULL_UP) block->PUPDR |= (1 << pos);
  else if (pupd == PUPD_PULL_DOWN) block->PUPDR |= (2 << pos);
}

void gpio_configure_in_analog(GPIO_TypeDef *block, uint8_t pin) {
	int8_t channel = gpio_analog_channel(block, pin);
	if (channel == -1)
		return;
	
	gpio_enable_clock(block);
	
	uint32_t pos = pin * 2;
	block->MODER |= 3 << pos; // Analog mode.
}

void gpio_write(GPIO_TypeDef *block, uint8_t pin, bool value) {
	if (value)
		block->ODR |= 0b1 << pin;
	else
		block->ODR &= ~(0b1 << pin);
}

bool gpio_read(GPIO_TypeDef *block, uint8_t pin) {
	return (block->IDR & (0b1 << pin)) != 0;
}

uint16_t gpio_read_analog(GPIO_TypeDef *block, uint8_t pin) {
	int8_t channel = gpio_analog_channel(block, pin);
	if (channel == -1) return 0;
	
	return adc_read(channel);
}

void gpio_configure_interrupt(
	GPIO_TypeDef *block, 
	uint8_t pin, 
	gpio_exti_edge_t edge,
	gpio_pupd_t pupd,
	uint8_t priority,
	gpio_exti_cb_t callback
) {
	// SYSCFG Clock.
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	
	// GPIO In Configuration.
	gpio_configure_in(block, pin, pupd);
	
	// EXTICR Configuration.
	uint32_t exti_idx = pin / 4u; // EXTICR[0..3]
  uint32_t exti_shift = (pin % 4u) * 4u;
  uint32_t mask = (0xFu << exti_shift);
  SYSCFG->EXTICR[exti_idx] = (SYSCFG->EXTICR[exti_idx] & ~mask)
                           | (gpio_block_index(block) << exti_shift);

	// EXTI Configuration.
	uint32_t bit = (1u << pin);
	EXTI->IMR &= ~bit; // Mask while performing configuration.
	
	// Rising/Falling Edge Configuration.
	EXTI->RTSR &= ~bit;
  EXTI->FTSR &= ~bit;
	
	if (edge == EXTI_EDGE_RISING || edge == EXTI_EDGE_BOTH) EXTI->RTSR |= bit;
	if (edge == EXTI_EDGE_FALLING || edge == EXTI_EDGE_BOTH) EXTI->FTSR |= bit;

	EXTI->PR |= bit; // Clear pending.
	EXTI->IMR |= bit; // Unmask.
	
	// NVIC Configuration.
	IRQn_Type irqn = gpio_exti_line_to_irq(pin);
  NVIC_SetPriority(irqn, priority);
  NVIC_EnableIRQ(irqn);
	
	// Store the callback.
	s_exti_callbacks[pin] = callback;
}

void gpio_configure_alternate(GPIO_TypeDef *block, uint8_t pin, uint8_t fn) {
	gpio_enable_clock(block);
	
	// Set the MODER register for the pin to "alternate function".
	block->MODER &= ~(0b11 << (2 * pin));
	block->MODER |= (0b10 << (2 * pin));

	uint8_t afr_idx = pin / 8;
	uint8_t afr_shift = (pin % 8) * 4;

	block->AFR[afr_idx] &= ~(0b1111 << afr_shift);
	block->AFR[afr_idx] |= ((fn & 0b1111) << afr_shift);
}

static inline void handle_line(uint8_t line) {
  uint32_t bit = (1u << line);
  if (EXTI->PR & bit) {
    EXTI->PR = bit;              // clear by writing 1
    if (s_exti_callbacks[line]) s_exti_callbacks[line]();
  }
}

void EXTI0_IRQHandler(void) { handle_line(0);  }

void EXTI1_IRQHandler(void) { handle_line(1);  }

void EXTI2_IRQHandler(void) { handle_line(2);  }

void EXTI3_IRQHandler(void) { handle_line(3);  }

void EXTI4_IRQHandler(void) { handle_line(4);  }

void EXTI9_5_IRQHandler(void) { 
	for (uint8_t i = 5; i <= 9; i++)  handle_line(i); 
}

void EXTI15_10_IRQHandler(void) { 
	for (uint8_t i = 10; i <= 15; i++) handle_line(i); 
}