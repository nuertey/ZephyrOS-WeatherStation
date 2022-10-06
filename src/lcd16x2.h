/*
 * Copyright (c) 2022 Nuertey Odzeyem
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * LCDs (Liquid Crystal Displays) are used in embedded system applications
 * for displaying various parameters and statuses of the system. The LCD
 * 16x2 has the following characteristics:
 *
 *   - It is a 16-pin device that has 2 rows that can accommodate 16
 *     characters each. The pins and their corresponding functions are:
 *
 *      [a] VSS  - Ground
 *      [b] VCC  - +5V
 *      [c] VEE  - Contrast Control
 *      [d] RS   - Register Select
 *      [e] RW   - Read/Write
 *      [f] E    - Enable
 *      [g] D0   - Data Pin 0
 *      [h] D1   - Data Pin 1
 *      [i] D2   - Data Pin 2
 *      [j] D3   - Data Pin 3
 *      [k] D4   - Data Pin 4
 *      [l] D5   - Data Pin 5
 *      [m] D6   - Data Pin 6
 *      [n] D7   - Data Pin 7
 *      [o] LED+ - LED+ 5V
 *      [p] LED- - LED- Ground
 *
 *   - It can be used either in a 4-bit mode or an 8-bit mode.
 *
 *   - For displaying on it, custom characters can be created.
 *
 *   - And lastly, the LCD 16x2 has 8 data lines and 3 control lines
 *     that can be used for control purposes.
 */

/*
 * @file Display text strings on generic LCD16x2 LCD controller
 * using GPIO for parallel interface on MCU.
 *
 * Datasheet: http://wiki.sunfounder.cc/index.php?title=LCD1602_Module
 *
 * LCD Wiring
 * ----------
 * The wiring for the LCD is as follows:
 *
 * 1 : GND
 * 2 : 5V
 * 3 : Contrast (0-5V)*
 * 4 : RS (Register Select)
 * 5 : R/W (Read Write)       - GROUND THIS PIN
 * 6 : Enable or Strobe
 * 7 : Data Bit 0             - NOT USED
 * 8 : Data Bit 1             - NOT USED
 * 9 : Data Bit 2             - NOT USED
 * 10: Data Bit 3             - NOT USED
 * 11: Data Bit 4
 * 12: Data Bit 5
 * 13: Data Bit 6
 * 14: Data Bit 7
 * 15: LCD Backlight +5V**
 * 16: LCD Backlight GND
 *
 * Every 5 second you should see this repeatedly on display:
 * 
 *     Nuertey Odzeyem
 *     NUCLEO F767ZI
 */
#pragma once
 
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/gpio.h>
#include <string.h>

/* Choose pins on the same port so as to make coding logic straightforward. */
#define GPIO_NODE DT_NODELABEL(gpiof)

// TBD, Nuertey Odzeyem; If using 8-bit mode, must define these pins as 
// well. For now, and to encourage compilation, simply set to an UNUSED
// dummy GPIO pin on the same port. The following satisfies:
//
// Connector: CN10 
// Pin      : 2 
// Pin Name : D7        * Arduino-equivalent pin name
// STM32 Pin: PF13
// Signal   : I/O
#define GPIO_PIN_D0       13  /* NC in 4-bit mode */
#define GPIO_PIN_D1       13  /* NC in 4-bit mode */
#define GPIO_PIN_D2       13  /* NC in 4-bit mode */
#define GPIO_PIN_D3       13  /* NC in 4-bit mode */

#define GPIO_PIN_D4        8  /* PF8 */
#define GPIO_PIN_D5        2  /* PF2 */
#define GPIO_PIN_D6        1  /* PF1 */
#define GPIO_PIN_D7        0  /* PF0 */
#define GPIO_PIN_RS        9  /* PF9 */
#define GPIO_PIN_E         7  /* PF7 */
#define GPIO_NAME         "GPIO_"

/* Commands */
#define LCD_CLEAR_DISPLAY           0x01
#define LCD_RETURN_HOME             0x02
#define LCD_ENTRY_MODE_SET          0x04
#define LCD_DISPLAY_CONTROL         0x08
#define LCD_CURSOR_SHIFT            0x10
#define LCD_FUNCTION_SET            0x20
#define LCD_SET_CGRAM_ADDR          0x40
#define LCD_SET_DDRAM_ADDR          0x80

/* Display entry mode */
#define LCD_ENTRY_RIGHT             0x00
#define LCD_ENTRY_LEFT              0x02
#define LCD_ENTRY_SHIFT_INCREMENT   0x01
#define LCD_ENTRY_SHIFT_DECREMENT   0x00

/* Display on/off control */
#define LCD_DISPLAY_ON              0x04
#define LCD_DISPLAY_OFF             0x00
#define LCD_CURSOR_ON               0x02
#define LCD_CURSOR_OFF              0x00
#define LCD_BLINK_ON                0x01
#define LCD_BLINK_OFF               0x00

/* Display/cursor shift */
#define LCD_DISPLAY_MOVE            0x08
#define LCD_CURSOR_MOVE             0x00
#define LCD_MOVE_RIGHT              0x04
#define LCD_MOVE_LEFT               0x00

/* Function set */
#define LCD_8BIT_MODE               0x10
#define LCD_4BIT_MODE               0x00
#define LCD_2_LINE                  0x08
#define LCD_1_LINE                  0x00
#define LCD_5x10_DOTS               0x04
#define LCD_5x8_DOTS                0x00

/* Define some device constants */
#define LCD_WIDTH                     16  /* Max char per line */
#define HIGH                           1
#define LOW                            0

/* in millisecond */
#define ENABLE_DELAY                  10

/*************************
 * USER can use these APIs
 *************************/
void pi_lcd_home(const struct device *gpio_dev);
void pi_lcd_set_cursor(const struct device *gpio_dev, uint8_t col, uint8_t row);
void pi_lcd_clear(const struct device *gpio_dev);
void pi_lcd_display_on(const struct device *gpio_dev);
void pi_lcd_display_off(const struct device *gpio_dev);
void pi_lcd_cursor_off(const struct device *gpio_dev);
void pi_lcd_cursor_on(const struct device *gpio_dev);
void pi_lcd_blink_off(const struct device *gpio_dev);
void pi_lcd_blink_on(const struct device *gpio_dev);
void pi_lcd_scroll_left(const struct device *gpio_dev);
void pi_lcd_scroll_right(const struct device *gpio_dev);
void pi_lcd_left_to_right(const struct device *gpio_dev);
void pi_lcd_right_to_left(const struct device *gpio_dev);
void pi_lcd_auto_scroll_right(const struct device *gpio_dev);
void pi_lcd_auto_scroll_left(const struct device *gpio_dev);
void pi_lcd_string(const struct device *gpio_dev, char *msg);
void pi_lcd_init(const struct device *gpio_dev, uint8_t cols, uint8_t rows, uint8_t dotsize);
