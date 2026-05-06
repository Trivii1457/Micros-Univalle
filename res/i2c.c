#include "i2c.h"
#include <avr/io.h>
#include <util/delay.h>

#define LCD_ADDR 0x4E

void i2c_init(void) {
    TWSR = 0x00;
    TWBR = 92;
    TWCR = (1 << TWEN);
}

void i2c_start(void) {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}

void i2c_stop(void) {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

void i2c_write(uint8_t data) {
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}

void i2c_lcd_send_nibble(uint8_t nibble, uint8_t rs) {
    uint8_t data = (nibble & 0xF0) | (1 << 3) | (rs & 0x01);
    i2c_start();
    i2c_write(LCD_ADDR);
    i2c_write(data | (1 << 2));
    _delay_us(1);
    i2c_write(data & ~(1 << 2));
    _delay_us(50);
    i2c_stop();
}

void i2c_lcd_send_byte(uint8_t byte, uint8_t is_data) {
    i2c_lcd_send_nibble(byte & 0xF0, is_data);
    i2c_lcd_send_nibble((byte << 4) & 0xF0, is_data);
}

void i2c_lcd_cmd(uint8_t cmd) {
    i2c_lcd_send_byte(cmd, 0);
}

void i2c_lcd_data(uint8_t data) {
    i2c_lcd_send_byte(data, 1);
}

void i2c_lcd_init(void) {
    i2c_init();
    _delay_ms(50);
    i2c_lcd_send_nibble(0x30, 0);
    _delay_ms(5);
    i2c_lcd_send_nibble(0x30, 0);
    _delay_us(150);
    i2c_lcd_send_nibble(0x30, 0);
    _delay_ms(1);
    i2c_lcd_send_nibble(0x20, 0);
    _delay_ms(1);
    i2c_lcd_cmd(0x28);
    i2c_lcd_cmd(0x0C);
    i2c_lcd_cmd(0x01);
    _delay_ms(2);
}

void i2c_lcd_clear(void) {
    i2c_lcd_cmd(0x01);
    _delay_ms(2);
}

void i2c_lcd_puts(char *str) {
    while (*str) {
        i2c_lcd_data(*str++);
    }
}

void i2c_lcd_set_cursor(uint8_t row, uint8_t col) {
    uint8_t row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
    i2c_lcd_cmd(0x80 | (row_offsets[row - 1] + (col - 1)));
}

void i2c_lcd_printf(char *str, ...) {
    char buffer[21];
    va_list args;
    va_start(args, str);
    vsnprintf(buffer, 21, str, args);
    va_end(args);
    i2c_lcd_puts(buffer);
}
