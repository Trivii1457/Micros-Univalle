#include "menu.h"
#include "botones.h"
#include "reloj.h"
#include "../lib/LCD.h"
#include "../lib/gpio.h"
#include <stdio.h>

typedef enum {
    EST_RELOJ,
    EST_MENU,
    EST_VER_DHT,
    EST_VER_LDR,
    EST_AJUSTAR_HORA,
} MenuState;

static MenuState estado = EST_RELOJ;
static uint8_t pantalla_sucia = 1; //Cuando la pantalla necesite ser actualizada
static uint8_t cursor_menu = 0; 
static uint8_t indice_vista = 0;

static uint8_t h_temp, m_temp, s_temp; 
static uint8_t campo_ajuste = 0; //0=horas, 1=minutos, 2=segundos

static uint8_t seg_anterior = 0xFF;

#define Max_menu 3
static const char *opciones_menu[Max_menu] = {
    "1. Ver DHT11",
    "2. Ver LDR",
    "3. Ajustar hora"
};

static void cambiar_estado(MenuState nuevo_estado) {
    lcd_clear();
    estado = nuevo_estado;
    pantalla_sucia = 1;
    indice_vista = 0;
}

static void dibujar_reloj(void)
{
    uint8_t min_muestra;
    //Fila 1
    lcd_set_cursor(1, 1);
    lcd_print("Hora: %02d:%02d:%02d", Hora_actual.horas, Hora_actual.minutos, Hora_actual.segundos);

    min_muestra =  0x00;

}