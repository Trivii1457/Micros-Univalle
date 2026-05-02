/*
    @Author: Trivi
*/


#include "botones.h"


void Botones_Init(void){
    GPIO_Init(Botones_Puerto, Boton_1, INPUT);
    GPIO_Init(Botones_Puerto, Boton_2, INPUT);
    GPIO_Init(Botones_Puerto, Boton_3, INPUT);
    GPIO_Init(Botones_Puerto, Boton_4, INPUT);


    //Como los botones los trabajamos pull up, los ponemos en alto
    GPIO_WritePin(Botones_Puerto, Boton_1, HIGH);   
    GPIO_WritePin(Botones_Puerto, Boton_2, HIGH);
    GPIO_WritePin(Botones_Puerto, Boton_3, HIGH);
    GPIO_WritePin(Botones_Puerto, Boton_4, HIGH);
}



//Funciones para leer los botones, retornan 1 si el boton esta presionado, 0 si no lo esta
uint8_t arriba(void){
    return GPIO_ReadPin(Botones_Puerto, Boton_1);
}

uint8_t abajo(void){
    return GPIO_ReadPin(Botones_Puerto, Boton_2);
}

uint8_t izquierda(void){
    return GPIO_ReadPin(Botones_Puerto, Boton_3);
}

uint8_t derecha(void){
    return GPIO_ReadPin(Botones_Puerto, Boton_4);
}
