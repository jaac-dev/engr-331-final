#include "delay.h"
#include "lcd.h"
#include "gpio.h"
#include "keypad.h"
#include "ultrasonic.h"
#include "systick.h"
#include "config.h"

#include "core_cm4.h"

#include <stdio.h>


uint8_t active_key;
const char KEYPAD_LAYOUT[12] = {'1', '2', '3',
																'4', '5', '6', 
																'7', '8', '9', 
																'*', '0', '#'};

void col1_handler();
void col2_handler();
void col3_handler();

int fputc(int ch, FILE *f) {
	(void) f;
	ITM_SendChar((uint32_t)ch);   // goes out on SWO
	return ch;
}

/// Globals
lcd_t g_lcd_a;
lcd_t g_lcd_b;
ultrasonic_t g_ultrasonic;

#define DISTANCE_POLL_TIME 1000
uint32_t g_last_distance_poll = 0;
float g_distance = 0.0f;

/// Main entrypoint for the firmware.
int main() {
	// Initialize the high speed clock.
	hs_clock_init();
	
	// Initialize the systick system.
	systick_init();
	
	// Initialize the delay system.
	delay_init();
	
	// Initialize LCD A and LCD B.
	g_lcd_b = lcd_def(
		LCD_A_RS_BLOCK, LCD_A_RS_PIN,
		LCD_A_EN_BLOCK, LCD_A_EN_PIN,
		LCD_A_D4_BLOCK, LCD_A_D4_PIN,
		LCD_A_D5_BLOCK, LCD_A_D5_PIN,
		LCD_A_D6_BLOCK, LCD_A_D6_PIN,
		LCD_A_D7_BLOCK, LCD_A_D7_PIN
	);
	
	g_lcd_a = lcd_def(
		LCD_B_RS_BLOCK, LCD_B_RS_PIN,
		LCD_B_EN_BLOCK, LCD_B_EN_PIN,
		LCD_B_D4_BLOCK, LCD_B_D4_PIN,
		LCD_B_D5_BLOCK, LCD_B_D5_PIN,
		LCD_B_D6_BLOCK, LCD_B_D6_PIN,
		LCD_B_D7_BLOCK, LCD_B_D7_PIN
	);
	
	// Clear the LCDs.
	lcd_command_clear(&g_lcd_a);
	lcd_command_clear(&g_lcd_b);
	
	// Configure keypad
	keypad_init();
	keypad_interrupt(col1_handler,col2_handler,col3_handler); // interrupt config.
	
	// Initialize the ultrasonic sensor.
	{
		RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	
		ultrasonic_pin_t usp_echo;
		usp_echo.block = GPIOA;
		usp_echo.pin = 0;
		
		ultrasonic_pin_t usp_trig;
		usp_trig.block = GPIOA;
		usp_trig.pin = 1;
		
		g_ultrasonic.echo = usp_echo;
		g_ultrasonic.trig = usp_trig;
		g_ultrasonic.echo_af = 1;
		g_ultrasonic.timer = TIM2;
		g_ultrasonic.timer_channel = 1;
		
		ultrasonic_init(&g_ultrasonic);
	}
		
	__enable_irq();
	
	g_distance = 0.0f;
	
	// Main loop.
	while (1) {
		// Capture the current time.
		
		uint32_t current_time = systick_get();
		
		// Check if we need to poll distance.
		if (current_time - g_last_distance_poll >= DISTANCE_POLL_TIME) {
			g_last_distance_poll = current_time;
			g_distance = ultrasonic_read(&g_ultrasonic);
			
			lcd_command_clear(&g_lcd_a);
			lcd_printf(&g_lcd_a, "Dist: %.2fcm", g_distance);
		}
		
		if (active_key != 0) {
			if (active_key == 10)
				lcd_command_clear(&g_lcd_b);
			else if (active_key == 12)
				lcd_print(&g_lcd_b,":)");
			else	
				lcd_print_char(&g_lcd_b,KEYPAD_LAYOUT[active_key-1]);
			active_key = 0;
		}
	}
	
	return 0;
}

void TIM2_IRQHandler() {
	if (TIM2->SR & TIM_SR_CC1IF) {
		ultrasonic_interrupt(&g_ultrasonic);
		TIM2->SR &= ~TIM_SR_CC1IF;
	}
}

void col1_handler() {
	uint8_t scan = keypad_scan(1,c1);
	if (keypad_debounce(c1))
		active_key = scan;
}

void col2_handler(){
	uint8_t scan = keypad_scan(2,c2);
	if (keypad_debounce(c2))
		active_key = scan;
}

void col3_handler(){
	uint8_t scan = keypad_scan(3,c3);
	if (keypad_debounce(c3))
		active_key = scan;
}
	