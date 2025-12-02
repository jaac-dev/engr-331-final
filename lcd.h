#pragma once

#include <stm32f407xx.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "delay.h"
#include "gpio.h"

/// The configuration structure used for LCD functions.
typedef struct {
	GPIO_TypeDef *rs_block;
	uint8_t rs_pin;
	
	GPIO_TypeDef *en_block;
	uint8_t en_pin;
	
	GPIO_TypeDef *d4_block;
	uint8_t d4_pin;
	
	GPIO_TypeDef *d5_block;
	uint8_t d5_pin;
	
	GPIO_TypeDef *d6_block;
	uint8_t d6_pin;
	
	GPIO_TypeDef *d7_block;
	uint8_t d7_pin;
} lcd_t;

/// Function for initializing an LCD using its respective pin numbers and GPIO.
lcd_t lcd_def(
	GPIO_TypeDef *rs_block, uint8_t rs_pin, 
	GPIO_TypeDef *en_block, uint8_t en_pin, 
	GPIO_TypeDef *d4_block, uint8_t d4_pin,
	GPIO_TypeDef *d5_block, uint8_t d5_pin,
	GPIO_TypeDef *d6_block, uint8_t d6_pin,
	GPIO_TypeDef *d7_block, uint8_t d7_pin
);

//=======================================//
//            LCD: RAW ACCESS            //
//=======================================//

/// Initializes the LCD. Pins must be configured prior to calling this.
/// @arg config The configuration object.
void lcd_init(lcd_t *self);

/// Writes a nibble to the LCD.
/// @arg self The LCD instance.
/// @arg nibble The nibble to write.
/// @arg rs Whether or not to have RS pulled high.
void lcd_write_nibble(lcd_t *self, uint8_t nibble, bool rs);

/// Writes a byte to the LCD.
/// @arg self The LCD instance.
/// @arg data The data to write.
/// @arg rs Whether or not to have RS pulled high.
void lcd_write(lcd_t *self, uint8_t data, bool rs);


//=======================================//
//             LCD: COMMANDS             //
//=======================================//

/// Issues the clear display command.
/// @arg self The LCD instance.
void lcd_command_clear(lcd_t *self);

/// Issues the return home command.
/// @arg self The LCD instance.
void lcd_command_return_home(lcd_t *self);

/// Issues the entry mode set command.
/// @arg self The LCD instance.
/// @arg id
/// @arg s
void lcd_command_entry_mode_set(lcd_t *self, bool id, bool s);

/// Issues the display control command.
/// @arg self The LCD instance.
/// @arg d
/// @arg c
/// @arg b
void lcd_command_display_control(lcd_t *self, bool d, bool c, bool b);

/// Issues the display/cursor shift command.
/// @arg self The LCD instance.
/// @arg sc
/// @arg rl
void lcd_command_shift(lcd_t *self, bool sc, bool rl);

/// Issues the function set command.
/// @arg self The LCD instance.
/// @arg dl
/// @arg n
/// @arg f
void lcd_command_function_set(lcd_t *self, bool dl, bool n, bool f);

/// Issues the DDRAM address command.
/// @arg self The LCD instance.
/// @arg addr
void lcd_command_ddram_address(lcd_t *self, uint8_t addr);


//=======================================//
//             LCD: FRONTEND             //
//=======================================//

/// Prints a message to the LCD.
/// @arg self The LCD instance.
/// @arg data The string to print.
void lcd_print(lcd_t *self, const char *data);

/// Prints a single character to the LCD.
/// @arg self The LCD instance.
/// @arg data The character to print.
void lcd_print_char(lcd_t *self, char data);

void lcd_printf(lcd_t *self, const char *fmt, ...);

/// Sets the position of the cursor on the LCD.
/// @arg self The LCD instance.
/// @arg col The column.
/// @arg row Whether or not to be on the second row.
void lcd_set_pos(lcd_t *self, uint8_t col, bool row);

/// function for printing an integer
/// @arg self The LCD instance.
/// @arg num The single-digit int
void lcd_print_int(lcd_t *self, uint8_t num);