/* 
 @author: Trivi
 
 Nota personal: Entre la desesperacion amorosa nada mejor que programar un micro avr hasta el amanecer
 Pd: Mar, aun te amo y te extraño, espero que estes bien y feliz, te deseo lo mejor del mundo, siempre seras mi persona favorita, 
    gracias por haberme dado la oportunidad de conocerte y compartir momentos tan lindos juntos, te amo <3

*/

#ifndef RELOJ_H
#define RELOJ_H

#include <stdint.h>


#define Max_muestras 5
#define intervalo 5 //Esra representado por minutos en el reloj

typedef struct{
    uint8_t horas;
    uint8_t minutos;
    uint8_t segundos;
} Tiempo;

typedef struct{
    Tiempo Hora;
    uint8_t Temperatura;
    uint8_t Humedad;
    uint16_t LDR;   
    uint8_t lec_ok; //1 Si la lectura es correcta, 0 si no lo es

} Muestra;


//Variables globales
extern Tiempo Hora_actual;
extern Muestra muestras[Max_muestras];
extern uint8_t numero_muestras; 
extern uint8_t indice_siguiente; 


//Funciones 
void Reloj_Init(uint8_t horas, uint8_t minutos, uint8_t segundos);

void tick_reloj(void);

void ajustar_reloj(uint8_t horas, uint8_t minutos, uint8_t segundos);

uint8_t muestra_pendiente_(void);

void guardar_muestra(uint8_t temperatura, uint8_t humedad, uint16_t ldr, uint8_t lec_ok);

Muestra* obtener_muestras(uint8_t idx);

#endif 