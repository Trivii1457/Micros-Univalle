/*
 * LCD_I2C.c
 *
 * Implementacion del driver LCD por I2C.
 * Ver LCD_I2C.h para descripcion del hardware y protocolo.
 *
 * Created: 2026
 * Author : Estudiante
 */

#include "i2c.h"

/* =====================================================
   Variables internas
   ===================================================== */

/* Guarda el estado actual de la iluminacion y los flags de pantalla */
static uint8_t _backlight  = LCD_BIT_BL;   /* encendida por defecto */
static uint8_t _display_ctrl;               /* bits D, C, B del registro Display Control */
static uint8_t _display_mode;               /* bits para Entry Mode Set */
static char    _buf[LCD_COLS + 1];          /* buffer para lcd_printf */

/* =====================================================
   Funciones privadas (solo visibles en este archivo)
   ===================================================== */

/*
 * pcf_write(byte)
 * Envia un byte directamente al PCF8574 por I2C.
 * Los 8 bits del byte se reflejan en los 8 pines de salida del PCF8574.
 * El bit de backlight se OR-ea siempre para no apagar la iluminacion
 * accidentalmente al enviar datos.
 */
static void pcf_write(uint8_t byte) {
    uint8_t out = byte | _backlight;
    TWI_WriteToAddress(LCD_I2C_ADDR, &out, 1);
}

/*
 * lcd_pulse_enable(byte)
 * Genera el pulso de Enable que necesita el HD44780 para leer un nibble.
 * El flanco de BAJADA de EN es cuando la LCD captura los datos en D4-D7.
 *
 * Secuencia:
 *   1. Enviar el byte con EN = 1 (preparar datos)
 *   2. Esperar un poco (setup time: minimo 450ns, usamos 1ms para ser seguros)
 *   3. Enviar el byte con EN = 0 (flanco de bajada: la LCD lee)
 *   4. Esperar el tiempo de hold (minimo 37us, usamos 0.5ms)
 */
static void lcd_pulse_enable(uint8_t byte) {
    pcf_write(byte | LCD_BIT_EN);     /* EN = 1 */
    _delay_ms(1);
    pcf_write(byte & ~LCD_BIT_EN);    /* EN = 0 -> LCD lee aqui */
    _delay_ms(1);
}

/*
 * lcd_send_nibble(nibble, mode)
 * Envia los 4 bits altos de 'nibble' a las lineas D4-D7 de la LCD.
 * Asume que 'nibble' ya tiene los bits en las posiciones D4-D7.
 */
static void lcd_send_nibble(uint8_t nibble) {
    pcf_write(nibble);
    lcd_pulse_enable(nibble);
}

/*
 * lcd_send_byte(value, mode)
 * Envia un byte completo a la LCD en modo 4 bits.
 * En modo 4 bits se necesitan DOS transferencias de nibble (4 bits cada una):
 *   Primera:  los 4 bits ALTOS  del byte (bits 7-4) -> van a D4-D7
 *   Segunda:  los 4 bits BAJOS  del byte (bits 3-0) -> van a D4-D7
 *
 * @param value  El byte a enviar (comando o caracter ASCII)
 * @param mode   0 = es un comando (RS=0), LCD_BIT_RS = es un dato (RS=1)
 */
static void lcd_send_byte(uint8_t value, uint8_t mode) {
    /*
     * Construir el nibble alto: tomar bits 7-4 del valor,
     * ponerlos en las posiciones D4-D7 del PCF8574.
     * OR con 'mode' para poner RS=0 (comando) o RS=1 (dato).
     */
    uint8_t high_nibble = (value & 0xF0) | mode;

    /*
     * Construir el nibble bajo: desplazar los bits 3-0 a las
     * posiciones 7-4 para que coincidan con D4-D7.
     */
    uint8_t low_nibble  = ((value << 4) & 0xF0) | mode;

    lcd_send_nibble(high_nibble);
    lcd_send_nibble(low_nibble);
}

/* =====================================================
   Funciones publicas
   ===================================================== */

void lcd_command(uint8_t cmd) {
    lcd_send_byte(cmd, 0);   /* mode = 0: RS = 0 = comando */
}

void lcd_write(uint8_t letra) {
    lcd_send_byte(letra, LCD_BIT_RS);   /* RS = 1 = dato */
}

void lcd_init(void) {
    /*
     * Inicializacion del HD44780 en modo 4 bits.
     * Siguiendo la secuencia de reset por software del datasheet.
     * Esto es necesario porque al encender el micro el LCD puede
     * estar en estado indeterminado.
     */

    /* Inicializar I2C */
    TWI_Init();

    /* Esperar que el LCD termine su propio reset interno (>15ms) */
    _delay_ms(50);
    
    /* Configurar estado inicial del expansor con backlight, esperar 1 segundo por displays lentos */
    pcf_write(_backlight);
    _delay_ms(1000);

    /*
     * Secuencia de inicializacion especial:
     * Hay que enviar el nibble 0x03 tres veces para forzar el modo 8 bits
     * y luego cambiar a modo 4 bits con el nibble 0x02.
     * Esta secuencia funciona sin importar el estado previo del LCD.
     */
    lcd_send_nibble(0x30);   /* nibble 0011 -> modo 8 bits (primera vez) */
    _delay_ms(5);
    lcd_send_nibble(0x30);   /* nibble 0011 -> modo 8 bits (segunda vez) */
    _delay_ms(1);
    lcd_send_nibble(0x30);   /* nibble 0011 -> modo 8 bits (tercera vez) */
    _delay_ms(1);
    lcd_send_nibble(0x20);   /* nibble 0010 -> CAMBIAR a modo 4 bits */
    _delay_ms(1);

    /* Ahora el LCD ya esta en modo 4 bits, podemos usar lcd_command() */

    /* Configurar: 4 bits, 2 lineas, matriz 5x8 */
    lcd_command(LCD_FUNCTIONSET | LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS);
    _delay_us(37);

    /* Configurar display: encendido, con cursor y parpadeo */
    _display_ctrl = LCD_DISPLAY_ON | LCD_CURSOR_ON | LCD_BLINK_ON;
    lcd_command(LCD_DISPLAYCONTROL | _display_ctrl);
    _delay_us(37);

    /* Limpiar pantalla */
    lcd_command(LCD_CLEARDISPLAY);
    _delay_ms(2);   /* este comando tarda mas (~1.5ms) */

    /* Inicializar la direccion de texto (de izquierda a derecha por defecto) */
    _display_mode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    lcd_command(LCD_ENTRYMODESET | _display_mode);
    _delay_us(37);

    /* Encender el backlight */
    _backlight = LCD_BIT_BL;
    lcd_return_home(); /* Set cursor at 0,0 */
}

void lcd_puts(char *str) {
    while (*str) {
        lcd_write((uint8_t)(*str));
        str++;
    }
}

void lcd_set_cursor(uint8_t row, uint8_t col) {
    /*
     * Tabla de offsets de DDRAM por fila.
     * El HD44780 no almacena las filas de forma contigua en memoria:
     *   Fila 1: offset 0x00
     *   Fila 2: offset 0x40
     *   Fila 3: offset 0x14  (0x00 + 20 columnas)
     *   Fila 4: offset 0x54  (0x40 + 20 columnas)
     */
    static const uint8_t offsets[4] = {0x00, 0x40, 0x14, 0x54};

    if (row < 1 || row > 4) return;   /* fila invalida: ignorar */
    if (col < 1 || col > LCD_COLS) return;

    lcd_command(LCD_SET_DDRAM_ADDR | (offsets[row - 1] + (col - 1)));
}

void lcd_printf(char *str, ...) {
    va_list args;
    va_start(args, str);
    vsnprintf(_buf, LCD_COLS + 1, str, args);
    va_end(args);
    lcd_puts(_buf);
}

void lcd_clear(void) {
    lcd_command(LCD_CLEARDISPLAY);
    _delay_ms(2);
}

void lcd_return_home(void) {
    lcd_command(LCD_RETURNHOME);
    _delay_ms(2);
}

void lcd_backlight_on(void) {
    _backlight = LCD_BIT_BL;
    pcf_write(0x00);   /* refrescar el estado del PCF8574 */
}

void lcd_backlight_off(void) {
    _backlight = 0;
    pcf_write(0x00);
}

void lcd_on(void) {
    _display_ctrl |= LCD_DISPLAY_ON;
    lcd_command(LCD_DISPLAYCONTROL | _display_ctrl);
    _delay_us(37);
}

void lcd_off(void) {
    _display_ctrl &= (uint8_t)~LCD_DISPLAY_ON;
    lcd_command(LCD_DISPLAYCONTROL | _display_ctrl);
    _delay_us(37);
}

void lcd_enable_cursor(void) {
    _display_ctrl |= LCD_CURSOR_ON;
    lcd_command(LCD_DISPLAYCONTROL | _display_ctrl);
    _delay_us(37);
}

void lcd_disable_cursor(void) {
    _display_ctrl &= (uint8_t)~LCD_CURSOR_ON;
    lcd_command(LCD_DISPLAYCONTROL | _display_ctrl);
    _delay_us(37);
}

void lcd_enable_blink(void) {
    _display_ctrl |= LCD_BLINK_ON;
    lcd_command(LCD_DISPLAYCONTROL | _display_ctrl);
    _delay_us(37);
}

void lcd_disable_blink(void) {
    _display_ctrl &= (uint8_t)~LCD_BLINK_ON;
    lcd_command(LCD_DISPLAYCONTROL | _display_ctrl);
    _delay_us(37);
}

void lcd_scroll_left(void) {
    lcd_command(LCD_CURSORDISPLAYSHIFT | LCD_DISPLAY_SHIFT | LCD_MOVELEFT);
    _delay_us(37);
}

void lcd_scroll_right(void) {
    lcd_command(LCD_CURSORDISPLAYSHIFT | LCD_DISPLAY_SHIFT | LCD_MOVERIGHT);
    _delay_us(37);
}

void lcd_left_to_right(void) {
    _display_mode |= LCD_ENTRYLEFT;
    lcd_command(LCD_ENTRYMODESET | _display_mode);
    _delay_us(37);
}

void lcd_right_to_left(void) {
    _display_mode &= (uint8_t)~LCD_ENTRYLEFT;
    lcd_command(LCD_ENTRYMODESET | _display_mode);
    _delay_us(37);
}

void lcd_autoscroll(void) {
    _display_mode |= LCD_ENTRYSHIFTINCREMENT;
    lcd_command(LCD_ENTRYMODESET | _display_mode);
    _delay_us(37);
}

void lcd_no_autoscroll(void) {
    _display_mode &= (uint8_t)~LCD_ENTRYSHIFTINCREMENT;
    lcd_command(LCD_ENTRYMODESET | _display_mode);
    _delay_us(37);
}

void lcd_custom_char(uint8_t mem, uint8_t *charmap) {
    uint8_t i;
    lcd_command(LCD_SET_CGRAM_ADDR | ((mem & 0x07) << 3));
    for (i = 0; i < 8; i++) {
        lcd_write(charmap[i]);
    }
    lcd_command(LCD_SET_DDRAM_ADDR);   /* volver al modo DDRAM */
    _delay_us(37);
}