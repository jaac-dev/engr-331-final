#pragma once

#include <stm32f407xx.h>

/// LCD A
#define LCD_A_RS_BLOCK GPIOC
#define LCD_A_RS_PIN 0

#define LCD_A_EN_BLOCK GPIOC
#define LCD_A_EN_PIN 1

#define LCD_A_D4_BLOCK GPIOC
#define LCD_A_D4_PIN 2

#define LCD_A_D5_BLOCK GPIOC
#define LCD_A_D5_PIN 3

#define LCD_A_D6_BLOCK GPIOC
#define LCD_A_D6_PIN 4

#define LCD_A_D7_BLOCK GPIOC
#define LCD_A_D7_PIN 5


/// LCD B
#define LCD_B_RS_BLOCK GPIOC
#define LCD_B_RS_PIN 6

#define LCD_B_EN_BLOCK GPIOC
#define LCD_B_EN_PIN 7

#define LCD_B_D4_BLOCK GPIOC
#define LCD_B_D4_PIN 8

#define LCD_B_D5_BLOCK GPIOC
#define LCD_B_D5_PIN 9

#define LCD_B_D6_BLOCK GPIOC
#define LCD_B_D6_PIN 10

#define LCD_B_D7_BLOCK GPIOC
#define LCD_B_D7_PIN 11


// Keypad
#define KEYPAD_C1_BLOCK GPIOE
#define KEYPAD_C1_PIN 0

#define KEYPAD_C2_BLOCK GPIOE
#define KEYPAD_C2_PIN 1

#define KEYPAD_C3_BLOCK GPIOE
#define KEYPAD_C3_PIN 2

#define KEYPAD_R1_BLOCK GPIOE
#define KEYPAD_R1_PIN 3

#define KEYPAD_R2_BLOCK GPIOE
#define KEYPAD_R2_PIN 4

#define KEYPAD_R3_BLOCK GPIOE
#define KEYPAD_R3_PIN 5

#define KEYPAD_R4_BLOCK GPIOE
#define KEYPAD_R4_PIN 6

// Ultrasonic
#define ULTRASONIC_ECHO_BLOCK GPIOA
#define ULTRASONIC_ECHO_PIN 0

#define ULTRASONIC_TRIG_BLOCK GPIOA
#define ULTRASONIC_TRIG_PIN 1

// Light
#define LIGHT_BLOCK GPIOA
#define LIGHT_PIN 4

// Temperature
#define TEMPERATURE_BLOCK GPIOA
#define TEMPERATURE_PIN 3

// Tilt
#define TILT_BLOCK GPIOD
#define TILT_PIN 0

// Servo
#define SERVO_BLOCK GPIOD
#define SERVO_PIN 1