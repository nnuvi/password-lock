//#ifndef __LCD_I2C_H__
//#define __LCD_I2C_H__
//
//#include "stm32f1xx_hal.h"
//
//void lcd_init(void);
//void lcd_send_string(char *str);
//void lcd_send_data(char data);
//void lcd_send_cmd(char cmd);
//void lcd_send_command(uint8_t command);
//void lcd_set_cursor(uint8_t row, uint8_t col);
//void lcd_clear(void);
//
//#endif

#ifndef __LCD_I2C_H__
#define __LCD_I2C_H__

/**
 * @brief Includes the HAL driver present in the project
 */
#if __has_include("stm32f1xx_hal.h")
	#include "stm32f1xx_hal.h"
#elif __has_include("stm32c0xx_hal.h")
	#include "stm32c0xx_hal.h"
#elif __has_include("stm32g4xx_hal.h")
	#include "stm32g4xx_hal.h"
#endif

#include "stm32f1xx_hal.h"
/**
 * @brief Structure to hold LCD instance information
 */
typedef struct {
    I2C_HandleTypeDef *hi2c;     // I2C handler for communication
    uint8_t address;
    //uint8_t backlight; // NEW: 1 = ON, 0 = OFF// I2C address of the LCD
} I2C_LCD_HandleTypeDef;

/**
 * @brief Initializes the LCD.
 * @param lcd: Pointer to the LCD handle
 */
void lcd_init(I2C_LCD_HandleTypeDef *lcd);

/**
 * @brief Sends a command to the LCD.
 * @param lcd: Pointer to the LCD handle
 * @param cmd: Command byte to send
 */
void lcd_send_cmd(I2C_LCD_HandleTypeDef *lcd, char cmd);

/**
 * @brief Sends data (character) to the LCD.
 * @param lcd: Pointer to the LCD handle
 * @param data: Data byte to send
 */
void lcd_send_data(I2C_LCD_HandleTypeDef *lcd, char data);

/**
 * @brief Sends a single character to the LCD.
 * @param lcd: Pointer to the LCD handle
 * @param ch: Character to send
 */
void lcd_putchar(I2C_LCD_HandleTypeDef *lcd, char ch);

/**
 * @brief Sends a string to the LCD.
 * @param lcd: Pointer to the LCD handle
 * @param str: Null-terminated string to send
 */
void lcd_puts(I2C_LCD_HandleTypeDef *lcd, char *str);

/**
 * @brief Moves the cursor to a specific position on the LCD.
 * @param lcd: Pointer to the LCD handle
 * @param col: Column number (0-15)
 * @param row: Row number (0 or 1)
 */
void lcd_gotoxy(I2C_LCD_HandleTypeDef *lcd, int col, int row);

/**
 * @brief Clears the LCD display.
 * @param lcd: Pointer to the LCD handle
 */
void lcd_clear(I2C_LCD_HandleTypeDef *lcd);

#endif /* I2C_LCD_H */

