/*
 * LCD_I2C.h
 *
 * Driver para LCD HD44780 con interfaz I2C a traves del expansor PCF8574.
 *
 * Reemplaza al LCD.h original (que usaba 6 pines GPIO directos).
 * Las funciones publicas son exactamente las mismas: lcd_init(), lcd_puts(),
 * lcd_set_cursor(), etc. El resto del proyecto (menu.c, main.c) no cambia.
 *
 * Como funciona el modulo I2C de la LCD:
 *   El modulo tiene un chip PCF8574 soldado al dorso de la LCD.
 *   El PCF8574 es un expansor de GPIO por I2C: recibe 1 byte por I2C
 *   y sus 8 pines de salida reflejan los 8 bits de ese byte.
 *
 *   Mapeo de pines del PCF8574 a la LCD:
 *     P0 (bit 0) -> RS    (Register Select: 0=comando, 1=dato)
 *     P1 (bit 1) -> RW    (Read/Write: siempre en 0 = escritura)
 *     P2 (bit 2) -> EN    (Enable: pulso para que la LCD lea el nibble)
 *     P3 (bit 3) -> BL    (Backlight: 1=encendida, 0=apagada)
 *     P4 (bit 4) -> D4    (bit 4 del bus de datos)
 *     P5 (bit 5) -> D5    (bit 5 del bus de datos)
 *     P6 (bit 6) -> D6    (bit 6 del bus de datos)
 *     P7 (bit 7) -> D7    (bit 7 del bus de datos)
 *
 *   Para enviar un comando o dato a la LCD se necesitan 4 escrituras I2C:
 *     1. nibble alto + EN=1  (la LCD "lee" en el flanco de bajada de EN)
 *     2. nibble alto + EN=0
 *     3. nibble bajo + EN=1
 *     4. nibble bajo + EN=0
 *
 * Direccion I2C del PCF8574:
 *   Depende de los jumpers A0, A1, A2 del modulo:
 *   A2=0, A1=0, A0=0 -> 0x27  (la mas comun, jumpers sin soldar)
 *   A2=0, A1=1, A0=1 -> 0x3F  (algunos modulos chinos)
 *   Si no sabes la tuya, prueba 0x27 primero.
 *   Puedes hacer un scanner I2C para encontrarla (ver main.c).
 *
 * Created: 2026
 * Author : Estudiante
 */

#ifndef LCD_I2C_H_
#define LCD_I2C_H_

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <util/delay.h>
#include "twi.h"

/* =====================================================
   Configuracion: ajustar segun tu modulo
   ===================================================== */

/* Direccion I2C del modulo (probar 0x27 primero, si no funciona usar 0x3F) */
#define LCD_I2C_ADDR     0x3F

/* Dimensiones de la pantalla */
#define LCD_COLS         20
#define LCD_ROWS         4

/* =====================================================
   Bits del PCF8574 (no cambiar si usas el modulo estandar)
   ===================================================== */
#define LCD_BIT_RS    (1 << 0)   /* P0: Register Select */
#define LCD_BIT_RW    (1 << 1)   /* P1: Read/Write (siempre 0) */
#define LCD_BIT_EN    (1 << 2)   /* P2: Enable */
#define LCD_BIT_BL    (1 << 3)   /* P3: Backlight */
#define LCD_BIT_D4    (1 << 4)   /* P4: Data bit 4 */
#define LCD_BIT_D5    (1 << 5)   /* P5: Data bit 5 */
#define LCD_BIT_D6    (1 << 6)   /* P6: Data bit 6 */
#define LCD_BIT_D7    (1 << 7)   /* P7: Data bit 7 */

/* =====================================================
   Comandos del controlador HD44780
   (identicos al LCD.h original)
   ===================================================== */
#define LCD_CLEARDISPLAY      0x01
#define LCD_RETURNHOME        0x02
#define LCD_DISPLAYCONTROL    0x08
#define LCD_DISPLAY_ON        0x04
#define LCD_CURSOR_ON         0x02
#define LCD_BLINK_ON          0x01
#define LCD_FUNCTIONSET       0x20
#define LCD_4BITMODE          0x00
#define LCD_2LINE             0x08
#define LCD_5x8DOTS           0x00
#define LCD_SET_CGRAM_ADDR    0x40
#define LCD_SET_DDRAM_ADDR    0x80
#define LCD_CURSORDISPLAYSHIFT 0x10
#define LCD_DISPLAY_SHIFT     0x08
#define LCD_MOVERIGHT         0x04
#define LCD_MOVELEFT          0x00
#define LCD_ENTRYMODESET      0x04
#define LCD_ENTRYRIGHT        0x00
#define LCD_ENTRYLEFT         0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

/* =====================================================
   Funciones publicas (misma API que LCD.h original)
   ===================================================== */

void lcd_init(void);
void lcd_command(uint8_t cmd);
void lcd_write(uint8_t letra);

void lcd_puts(char *str);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_printf(char *str, ...);

void lcd_clear(void);
void lcd_return_home(void);

void lcd_on(void);
void lcd_off(void);
void lcd_backlight_on(void);
void lcd_backlight_off(void);

void lcd_enable_cursor(void);
void lcd_disable_cursor(void);
void lcd_enable_blink(void);
void lcd_disable_blink(void);

void lcd_scroll_left(void);
void lcd_scroll_right(void);

void lcd_left_to_right(void);
void lcd_right_to_left(void);
void lcd_autoscroll(void);
void lcd_no_autoscroll(void);

void lcd_custom_char(uint8_t mem, uint8_t *charmap);

#endif /* LCD_I2C_H_ */