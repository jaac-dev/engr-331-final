#include "i2c.h"

#include <stm32f407xx.h>

void i2c_init() {
	// Enable the clock for I2C1.
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	
	// Configure the GPIO ports.
	
}

void i2c_start_write(uint8_t addr);

void i2c_start_read(uint8_t addr);

void i2c_write_byte(uint8_t data);

void i2c_read_bytes(uint8_t *buf, uint32_t len);

void i2c_stop();