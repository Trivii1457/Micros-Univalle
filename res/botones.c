/*
    @Author: Trivi
*/


#include "botones.h"


void Botones_Init(void){
    HAL_GPIO_ConfigPin(Botones_Puerto, Boton_1, INPUT);
    HAL_GPIO_ConfigPin(Botones_Puerto, Boton_2, INPUT);
    HAL_GPIO_ConfigPin(Botones_Puerto, Boton_3, INPUT);
    HAL_GPIO_ConfigPin(Botones_Puerto, Boton_4, INPUT);


    //Como los botones los trabajamos pull up, los ponemos en alto
    HAL_GPIO_SetPullup(Botones_Puerto, Boton_1, HIGH);   
    HAL_GPIO_SetPullup(Botones_Puerto, Boton_2, HIGH);
    HAL_GPIO_SetPullup(Botones_Puerto, Boton_3, HIGH);
    HAL_GPIO_SetPullup(Botones_Puerto, Boton_4, HIGH);
}

static uint8_t tick_boton(uint8_t pin, uint16_t *contador, uint8_t *disparado) {
    if (HAL_GPIO_ReadPin(Botones_Puerto, pin) == LOW) {
        /* El boton esta presionado: contar milisegundos */
        if (*contador < 20) {
            (*contador)++;
        }
        /* Si llego a 20ms y aun no se ha reportado: confirmar pulsacion */
        if (*contador >= 20 && (*disparado == 0)) {
            *disparado = 1;
            return 1;   /* <<< aqui se detecta el boton */
        }
    } else {
        /* El boton fue soltado: resetear para la siguiente pulsacion */
        *contador  = 0;
        *disparado = 0;
    }
    return 0;
}

//Funciones para leer los botones
uint8_t arriba(void){
    static uint16_t cnt  = 0;
    static uint8_t  disp = 0;
    return tick_boton(Boton_1, &cnt, &disp);
}

uint8_t abajo(void){
    static uint16_t cnt  = 0;
    static uint8_t  disp = 0;
    return tick_boton(Boton_2, &cnt, &disp);
}

uint8_t seleccionar(void){
    static uint16_t cnt  = 0;
    static uint8_t  disp = 0;
    return tick_boton(Boton_3, &cnt, &disp);
}

uint8_t volver(void){
    static uint16_t cnt  = 0;
    static uint8_t  disp = 0;
    return tick_boton(Boton_4, &cnt, &disp);
}
