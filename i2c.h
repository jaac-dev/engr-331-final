#pragma once

#include <stdint.h>

void i2c_init();

void i2c_start_write(uint8_t addr);

void i2c_start_read(uint8_t addr);

void i2c_write_byte(uint8_t data);

void i2c_read_bytes(uint8_t *buf, uint32_t len);

void i2c_stop();