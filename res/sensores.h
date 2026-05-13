/* 
    @author: Trivi
*/
#ifndef SENSORES_H
#define SENSORES_H

#include <stdint.h>

#define Canal_HW870 1 // PA1
#define Canal_LDR 2   // PA2

void sensores_init(void);

uint16_t LDR_read(void);
uint16_t HW870_read(void);

#endif
