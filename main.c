#include "delay.h"
#include "lcd.h"
#include "gpio.h"
#include "keypad.h"
#include "ultrasonic.h"
#include "systick.h"

lcd_t lcd_a;
lcd_t lcd_b;

ultrasonic_t us;

volatile float distance = 0.0f;

void col1_handler();
void col2_handler();
void col3_handler();

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
	
	systick_init();
	
	// Initialize the delay system.
	delay_init();
	
	// Initialize LCD A and LCD B.
	//lcd_a = lcd_def(GPIOC, LCDa_RS, LCDa_EN, LCDa_D4, LCDa_D5, LCDa_D6, LCDa_D7);
	//lcd_b = lcd_def(GPIOC, LCDb_RS, LCDb_EN, LCDb_D4, LCDb_D5, LCDb_D6, LCDb_D7);
	
	// Clear the LCDs.
	//lcd_command_clear(&lcd_a);
	//lcd_command_clear(&lcd_b);
	
	//delay_ms(20);
	
	// Configure keypad
	//keypad_init();
	//keypad_interrupt(col1_handler,col2_handler,col3_handler); // interrupt config.
	
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	
	ultrasonic_pin_t usp_echo;
	usp_echo.block = GPIOA;
	usp_echo.pin = 0;
	
	ultrasonic_pin_t usp_trig;
	usp_trig.block = GPIOA;
	usp_trig.pin = 1;
	
	us.echo = usp_echo;
	us.trig = usp_trig;
	us.echo_af = 1;
	us.timer = TIM2;
	us.timer_channel = 1;
	
	ultrasonic_init(&us);
	
	__enable_irq();
	
	distance = 0.0f;
	
	while (1) {
		delay_ms(500);
		distance = ultrasonic_read(&us);
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

void TIM2_IRQHandler() {
	if (TIM2->SR & TIM_SR_CC1IF) {
		ultrasonic_interrupt(&us);
		TIM2->SR &= ~TIM_SR_CC1IF;
	}
}
