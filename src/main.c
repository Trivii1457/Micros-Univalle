#include "../lib/config.h"
#include "../lib/gpio.h"
#include "../lib/LCD.h"
#include "../res/sensores.h"
#include "../res/menu.h"
#include "../res/botones.h"
#include "../res/reloj.h"


int main(void) {

    lcd_init();
    lcd_disable_blink();
    lcd_set_cursor(1, 1);
    lcd_printf("   Reloj Digital    ");
    Botones_Init();
    ADC_init();
    Reloj_Init(0,0,0);
    menu_init();
    lcd_set_cursor(1, 1);
    lcd_puts("   Reloj Digital    ");
    lcd_clear();
    _delay_ms(2000);
    //while(1){tick_reloj;}
    
    while (1) {
        
        _delay_ms(5);
        
        
        tick_reloj(); 
        
        
        if (muestra_pendiente_()) {
            uint8_t  temp   = 0;
            uint8_t  hum    = 0;
            uint16_t ldr    = 0;
            uint8_t  dht_ok = 0;
            ldr = LDR_read();
 
            if (temp_read(&temp, &hum) == 0) {
                dht_ok = 1; 
            }
 
            guardar_muestra(temp, hum, ldr, dht_ok);
        }
        menu_update();
        tick_reloj();
    }
}

