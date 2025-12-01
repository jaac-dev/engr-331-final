#include "delay.h"
#include "lcd.h"
#include "gpio.h"
#include "keypad.h"

static volatile bool g_count_changed = false;
static volatile int g_count = 0;

lcd_t lcd_a;
lcd_t lcd_b;

void col1_handler();
void col2_handler();
void col3_handler();

static void user_button_handler() {
	g_count++;
	g_count_changed = true;
}

/// Initialize the high speed clock.
void hs_clock_init();

/// Initialize LCD A.
lcd_t lcd_a_init();

/// Initialize LCD B.
lcd_t lcd_b_init();

/// Main entrypoint for the firmware.
int main() {
	// Initialize the high speed clock.
	hs_clock_init();
	
	// Initialize the delay system.
	delay_init();
	
	// Initialize LCD A and LCD B.
	lcd_a = lcd_def(GPIOC,LCDa_RS, LCDa_EN, LCDa_D4, LCDa_D5, LCDa_D6, LCDa_D7);
	lcd_b = lcd_def(GPIOC,LCDb_RS, LCDb_EN, LCDb_D4, LCDb_D5, LCDb_D6, LCDb_D7);
	
	// Clear the LCDs.
	lcd_command_clear(&lcd_a);
	lcd_command_clear(&lcd_b);
	
	delay_ms(20);
	
	// Configure the user button as an interrupt.
	gpio_configure_interrupt(GPIOA, 0, EXTI_EDGE_RISING, PUPD_PULL_DOWN, 1, user_button_handler);
	
	const int A_OFFSET = 7;
	const int B_OFFSET = 11;
	
	lcd_print(&lcd_a, "Count: 0");
	lcd_print(&lcd_b, "Count * 8: 0");
	
	// Configure keypad
	keypad_init();
	keypad_interrupt(col1_handler,col2_handler,col3_handler); // interrupt config.

__enable_irq();
	
	while (1) {
		__WFI(); // Wait for interrupt.
	}
	return 0;
}

void col1_handler() {
	lcd_print_int(&lcd_a,keypad_scan(1));
}

void col2_handler() {
	lcd_print_int(&lcd_a,keypad_scan(2));
}

void col3_handler() {
	lcd_print_int(&lcd_a,keypad_scan(3));
}
