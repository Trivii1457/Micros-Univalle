/* 
    @author: Trivi
*/
#ifndef SENSORES_H
#define SENSORES_H

#include <stdint.h>

/*Voy a tratar de utilizar un sensor DHT11 y un fotoresistor*/

#define puerto_sensores 'C'
#define sensor_temp 0

#define Canal_LDR 2 //PA2

void ADC_init(void);

uint16_t LDR_read(void);

uint8_t temp_read(uint8_t *temp, uint8_t *humidity);



#endif
