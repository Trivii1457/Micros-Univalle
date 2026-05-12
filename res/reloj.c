/*
    @author: Trivi
 */

#include "reloj.h"


Tiempo Hora_actual = {0, 0, 0};
Muestra muestras[Max_muestras];
uint8_t numero_muestras = 0;
uint8_t indice_siguiente = 0;



static uint16_t ms_contador = 0;
static uint8_t muestra_pendiente = 0;

void Reloj_Init(uint8_t horas, uint8_t minutos, uint8_t segundos){
    Hora_actual.horas = horas % 24;
    Hora_actual.minutos = minutos % 60;
    Hora_actual.segundos = segundos % 60;
    ms_contador = 0;
    numero_muestras = 0;
    indice_siguiente = 0;
}

void ajustar_reloj(uint8_t horas, uint8_t minutos, uint8_t segundos){
    Hora_actual.horas = horas % 24;
    Hora_actual.minutos = minutos % 60;
    Hora_actual.segundos = segundos % 60;
    ms_contador = 0; // Reiniciar el contador de milisegundos al ajustar el reloj
}

//Tick del reloj, se debe llamar cada 5ms
void tick_reloj(void){
    ms_contador++;
    
    // 200 ticks de 5ms = 1000ms = 1 segundo en "tiempo real"
    if (ms_contador < 200) {
        return; 
    }
    ms_contador = 0;
    
    Hora_actual.segundos += 1;
    if (Hora_actual.segundos < 60) {
        return;
    }
    Hora_actual.segundos = 0;
    Hora_actual.minutos += 1; // 1 minuto real
    
    // Cada x intervalo, bandera de muestra
    if (Hora_actual.minutos % intervalo == 0 && Hora_actual.segundos == 0) {
        muestra_pendiente = 1;
    }

    if (Hora_actual.minutos < 60) {
        return;
    }
    Hora_actual.minutos = 0;
    Hora_actual.horas += 1;
    if (Hora_actual.horas < 24) {
        return;
    }
    Hora_actual.horas = 0;
}

uint8_t muestra_pendiente_(void){
    if (muestra_pendiente) {
        muestra_pendiente = 0; // Reiniciar el flag de muestra pendiente
        return 1;
    } else {
        return 0;
    }
}

void guardar_muestra(uint8_t temperatura, uint8_t humedad, uint16_t ldr, uint8_t lec_ok){
    //Me las hecho al bolsillo
    muestras[indice_siguiente].Hora = Hora_actual;
    muestras[indice_siguiente].Temperatura = temperatura;
    muestras[indice_siguiente].Humedad = humedad;
    muestras[indice_siguiente].LDR = ldr;
    muestras[indice_siguiente].lec_ok = lec_ok;
        
    indice_siguiente = (indice_siguiente + 1) % Max_muestras; // Avanzar al siguiente índice circularmente
    // Si el número de muestras es menor que el máximo, incrementarlo
    if (numero_muestras < Max_muestras) {
        numero_muestras++;
    }
}

Muestra* obtener_muestras(uint8_t idx){
    if (numero_muestras == 0 || idx >= numero_muestras) {
        return 0; // No hay muestras
    }
    uint8_t indice_real;
    if (numero_muestras < Max_muestras) {
        indice_real = idx; // Si no se ha llenado el buffer, el índice real es el mismo que el índice solicitado
    } else {
        indice_real = (indice_siguiente + idx) % Max_muestras; // Calcular el índice real en el buffer circular
    }
    return &muestras[indice_real];
}