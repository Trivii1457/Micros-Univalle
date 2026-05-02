/*
@Autor: Trivi
Libreria para segmentar los botones del menu para la LCD
*/

#ifndef BOTONES_H
#define BOTONES_H

#include <stdint.h>
#include "../lib/gpio.h"

//Defino los pines 
#define Boton_1 0
#define Boton_2 1
#define Boton_3 2
#define Boton_4 3


#define Botones_Puerto 'B'

//Inicializae botones
void Botones_Init(void);

//Ticks de los botones
uint8_t arriba(void);
uint8_t abajo(void);
uint8_t seleccionar(void);
uint8_t volver(void);

#endif 