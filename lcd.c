#include "lcd.h"

lcd_t lcd_def(
	GPIO_TypeDef *rs_block, uint8_t rs_pin, 
	GPIO_TypeDef *en_block, uint8_t en_pin, 
	GPIO_TypeDef *d4_block, uint8_t d4_pin,
	GPIO_TypeDef *d5_block, uint8_t d5_pin,
	GPIO_TypeDef *d6_block, uint8_t d6_pin,
	GPIO_TypeDef *d7_block, uint8_t d7_pin
) {
	// Configure the pins.
	gpio_configure_out(rs_block, rs_pin);
	gpio_configure_out(en_block, en_pin);
	gpio_configure_out(d4_block, d4_pin);
	gpio_configure_out(d5_block, d5_pin);
	gpio_configure_out(d6_block, d6_pin);
	gpio_configure_out(d7_block, d7_pin);
	
	// Setup the LCD.
	lcd_t lcd;
	lcd.rs_block = rs_block;
	lcd.rs_pin = rs_pin;
	lcd.en_block = en_block;
	lcd.en_pin = en_pin;
	lcd.d4_block = d4_block;
	lcd.d4_pin = d4_pin;
	lcd.d5_block = d5_block;
	lcd.d5_pin = d5_pin;
	lcd.d6_block = d6_block;
	lcd.d6_pin = d6_pin;
	lcd.d7_block = d7_block;
	lcd.d7_pin = d7_pin;

	lcd_init(&lcd); 
	
	return lcd;
}

void lcd_init(lcd_t *self) {
	// Initialize by instruction to get into 4-bit mode.
	lcd_write_nibble(self, 0b0011, false);
	
	delay_ms(20);
	
	lcd_write_nibble(self, 0b0011, false);
	
	delay_ms(20);
	
	lcd_write_nibble(self, 0b0011, false);	
	
	delay_ms(20);
	
	lcd_write_nibble(self, 0b0010, false);
	
	// Set the display mode.
	lcd_command_function_set(self, 0, 1, 0);
	
	delay_ms(20);
	
	// Turn the display off.
	lcd_command_display_control(self, 0, 0, 0);
	
	delay_ms(20);
	
	// Clear the display.
	lcd_command_clear(self);
	
	delay_ms(20);
	
	// Set entry mode.
	lcd_command_entry_mode_set(self, 1, 0);
	
	delay_ms(20);
	
	// Set display on.
	lcd_command_display_control(self, 1, 1, 1);
	
	delay_ms(20);
}

void lcd_write_nibble(lcd_t *self, uint8_t nibble, bool rs) {
	gpio_write(self->rs_block, self->rs_pin, rs ? 1 : 0);
	
	gpio_write(self->d7_block, self->d7_pin, (nibble & 8) != 0);
	gpio_write(self->d6_block, self->d6_pin, (nibble & 4) != 0);
	gpio_write(self->d5_block, self->d5_pin, (nibble & 2) != 0);
	gpio_write(self->d4_block, self->d4_pin, (nibble & 1) != 0);
	
	delay_ms(1); // Wait 1us.
	
	gpio_write(self->en_block, self->en_pin, 1);
	
	delay_ms(1); // Wait 1us.
	
	gpio_write(self->en_block, self->en_pin, 0);
	
	delay_ms(1); // Wait 1us.
}

void lcd_write(lcd_t *self, uint8_t data, bool rs) {
	lcd_write_nibble(self, data >> 4, rs);
	
	delay_ms(10);
	
	lcd_write_nibble(self, data & 0b1111, rs);
	
	delay_ms(10);
}

void lcd_command_clear(lcd_t *self) {
	lcd_write(self, 0b00000001, false);
}

void lcd_command_return_home(lcd_t *self) {
	lcd_write(self, 0b00000010, false);
}

void lcd_command_entry_mode_set(lcd_t *self, bool id, bool s) {
	uint8_t command = 0b00000100;
	
	if (id)
		command |= 2;
	if (s)
		command |= 1;
	
	lcd_write(self, command, false);
}

void lcd_command_display_control(lcd_t *self, bool d, bool c, bool b) {
	uint8_t command = 0b00001000;
	
	if (d)
		command |= 4;
	if (c)
		command |= 2;
	if (b)
		command |= 1;
	
	lcd_write(self, command, false);
}

void lcd_command_shift(lcd_t *self, bool sc, bool rl) {
	uint8_t command = 0b00010000;
	
	if (sc)
		command |= 8;
	if (rl)
		command |= 4;
	
	lcd_write(self, command, false);
}

void lcd_command_function_set(lcd_t *self, bool dl, bool n, bool f) {
	uint8_t command = 0b00100000;
	
	if (dl)
		command |= 16;
	if (n)
		command |= 8;
	if (f)
		command |= 4;
	
	lcd_write(self, command, false);
}

void lcd_command_ddram_address(lcd_t *self, uint8_t addr) {
	uint8_t command = 0b10000000 | (addr & 0b01111111);
	lcd_write(self, command, false);
}

void lcd_print(lcd_t *self, const char *data) {
	size_t len = strlen(data);
	
	for (size_t i = 0; i < len; i++)
		lcd_print_char(self, data[i]);
}

void lcd_print_char(lcd_t *self, char data) {
	lcd_write(self, data, true);
	delay_ms(2);
}

void lcd_printf(lcd_t *self, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	
	char buffer[256];
	int n_written = vsnprintf(buffer, 256, fmt, args);
	buffer[n_written] = '\0';
	
	va_end(args);
	
	lcd_print(self, buffer);
}

void lcd_set_pos(lcd_t *self, uint8_t col, bool row) {
	lcd_command_ddram_address(self, row ? col + 40 : col);
}

void lcd_print_int(lcd_t *self, uint8_t num) {
	char num_buffer[32] = "";
	snprintf(num_buffer,32,"%d",num);
	lcd_print(self,num_buffer);
}