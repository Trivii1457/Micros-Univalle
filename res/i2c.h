#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_write(uint8_t data);

void i2c_lcd_init(void);
void i2c_lcd_clear(void);
void i2c_lcd_puts(char *str);
void i2c_lcd_set_cursor(uint8_t row, uint8_t col);
void i2c_lcd_printf(char *str, ...);

#endif
