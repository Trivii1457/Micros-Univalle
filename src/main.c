#include "../lib/config.h"
#include "../lib/gpio.h"
#include "../lib/LCD.h"


int main(void) {
   
    

     

    ADMUX = 0x43; // AVcc referencia, canal ADC3

    ADCSRA = 0x87; // ADC enable, prescaler 128


    while (1) {
        printf("Hola mundo!\n");
        
    }
}

