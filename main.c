#include "delay.h"
#include "lcd.h"
#include "gpio.h"

static volatile bool g_count_changed = false;
static volatile int g_count = 0;

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
	lcd_t lcd_a = lcd_a_init();
	lcd_t lcd_b = lcd_b_init();
	
	// Clear the LCDs.
	lcd_command_clear(&lcd_a);
	lcd_command_clear(&lcd_b);
	
	delay_ms(20);
	
	// Configure the user button as an interrupt.
	gpio_configure_interrupt(GPIOA, 0, EXTI_EDGE_RISING, PUPD_PULL_UP, 1, user_button_handler);
	
	const int A_OFFSET = 7;
	const int B_OFFSET = 11;
	
	lcd_print(&lcd_a, "Count: 0");
	lcd_print(&lcd_b, "Count * 2: 0");
	
	while (1) {
		if (!g_count_changed) continue;
		g_count_changed = false; // Handle the change.
		
		// Output the new number.
		lcd_set_pos(&lcd_a, A_OFFSET, false);
		lcd_set_pos(&lcd_b, B_OFFSET, false);
		
		// Clear the remaining 40 - A_OFFSET or B_OFFSET chars.
		for (int i = 0; i < 40 - A_OFFSET; i++)
			lcd_print_char(&lcd_a, ' ');
		
		for (int i = 0; i < 40 - B_OFFSET; i++)
			lcd_print_char(&lcd_b, ' ');
		
		lcd_set_pos(&lcd_a, A_OFFSET, false);
		lcd_set_pos(&lcd_b, B_OFFSET, false);
		
		lcd_printf(&lcd_a, "%d", g_count);
		lcd_printf(&lcd_b, "%d", g_count * 2);
	}
	
	return 0;
}

void hs_clock_init() {
	// Enable the high speed oscillator.
	RCC->CR |= RCC_CR_HSEON;
	while ((RCC->CR & RCC_CR_HSERDY) == 0) {}
		
	// Set the power scale.
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR |= PWR_CR_VOS;

	// Configure the flash latency.
	FLASH->ACR =
		FLASH_ACR_ICEN |
		FLASH_ACR_DCEN |
		FLASH_ACR_PRFTEN |
    FLASH_ACR_LATENCY_5WS;
	while ((FLASH->ACR & FLASH_ACR_LATENCY) != FLASH_ACR_LATENCY_5WS) {}

	// Configure the prescalars.
	RCC->CFGR &= ~(RCC_CFGR_HPRE  |
	  RCC_CFGR_PPRE1 |
	  RCC_CFGR_PPRE2);
		
	RCC->CFGR |=
		RCC_CFGR_HPRE_DIV1  |  // AHB  = 168 MHz
		RCC_CFGR_PPRE1_DIV4 |  // APB1 = 42 MHz
		RCC_CFGR_PPRE2_DIV2;   // APB2 = 84 MHz

	// Configure the PLL.
	RCC->CR &= ~RCC_CR_PLLON;
  while (RCC->CR & RCC_CR_PLLRDY) {}

	RCC->PLLCFGR = 0;

	// PLLM = 8
	RCC->PLLCFGR |= 8U << 0;
	// PLLN = 336
	RCC->PLLCFGR |= 336U << 6;
	// PLLP = 2  -> PLLP bits = 00
	// (already 0 from clearing, so nothing to do)
	// PLL source = HSE
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE;
	// PLLQ = 7  -> 48 MHz for USB, etc.
	RCC->PLLCFGR |= 7U << 24;

	// 6. Enable PLL and wait
	RCC->CR |= RCC_CR_PLLON;
	while ((RCC->CR & RCC_CR_PLLRDY) == 0) {}

	// 7. Switch SYSCLK to PLL
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL;   // select PLL as system clock

	// Wait until PLL is used as system clock
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}
	
  // Update the CMSIS core clock.
	SystemCoreClock = 168000000U;
}

lcd_t lcd_a_init() {
	// Configure the pins.
	gpio_configure_out(GPIOC, 6);
	gpio_configure_out(GPIOC, 7);
	gpio_configure_out(GPIOC, 8);
	gpio_configure_out(GPIOC, 9);
	gpio_configure_out(GPIOC, 10);
	gpio_configure_out(GPIOC, 11);
	
	// Setup the LCD.
	lcd_t lcd_a;
	lcd_a.rs_block = GPIOC;
	lcd_a.rs_pin = 6;
	lcd_a.en_block = GPIOC;
	lcd_a.en_pin = 7;
	lcd_a.d4_block = GPIOC;
	lcd_a.d4_pin = 8;
	lcd_a.d5_block = GPIOC;
	lcd_a.d5_pin = 9;
	lcd_a.d6_block = GPIOC;
	lcd_a.d6_pin = 10;
	lcd_a.d7_block = GPIOC;
	lcd_a.d7_pin = 11;

	lcd_init(&lcd_a);
	
	return lcd_a;
}

lcd_t lcd_b_init() {
	// Configure the pins.
	gpio_configure_out(GPIOD, 0);
	gpio_configure_out(GPIOD, 1);
	gpio_configure_out(GPIOD, 2);
	gpio_configure_out(GPIOD, 3);
	gpio_configure_out(GPIOD, 4);
	gpio_configure_out(GPIOD, 5);
	
	// Setup the LCD.
	lcd_t lcd_b;
	lcd_b.rs_block = GPIOD;
	lcd_b.rs_pin = 0;
	lcd_b.en_block = GPIOD;
	lcd_b.en_pin = 1;
	lcd_b.d4_block = GPIOD;
	lcd_b.d4_pin = 2;
	lcd_b.d5_block = GPIOD;
	lcd_b.d5_pin = 3;
	lcd_b.d6_block = GPIOD;
	lcd_b.d6_pin = 4;
	lcd_b.d7_block = GPIOD;
	lcd_b.d7_pin = 5;

	lcd_init(&lcd_b);
	
	return lcd_b;
}