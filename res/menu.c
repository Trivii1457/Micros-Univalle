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

    //Fila 2
    min_muestra =  intervalo - (Hora_actual.minutos % intervalo);

    if (min_muestra == intervalo) {
        min_muestra = 0;
    }

    lcd_set_cursor(2, 1);
    lcd_print("Prox muestra en: %02d min", min_muestra);

    //Fila 3
    lcd_set_cursor(3, 1);
    lcd_print("Muestras: %d de %d", numero_muestras, Max_muestras);

    //Fila 4
    lcd_set_cursor(4, 1);
    lcd_printf(" [SEL] = Menu");

}

static void dibujar_menu(void)
{ 
    lcd_set_cursor(1, 1);
    lcd_printf("   MENU PRINCIPAL   ");
    for (uint8_t i = 0; i < Max_menu; i++) {
        lcd_set_cursor(2 + i, 1);
        if (i == cursor_menu) {
            lcd_printf("> %s", opciones_menu[i]);
        } else {
            lcd_printf("  %s", opciones_menu[i]);
        }
    }
}

static void dibujar_dht(void)
{
    Muestra *m;

    lcd_sert_cursor(1, 1);
    if (numero_muestras == 0) {
        lcd_printf("No hay muestras");
    }
    else {lcd_printf("DHT11 %d/%d [B]=menu ", indice_vista + 1, numero_muestras);}

    if (numero_muestras == 0)
    {
        lcd_set_cursor(2,1);
        lcd_printf("Aun no hay muestras");
        lcd_set_cursor(3,1);
        lcd_printf(" Esperar %d min ", intervalo);
        lcd_set_cursor(4,1);
        lcd_printf("  [BACK] = Volver   ");
        return;
    }

    m = obtener_muestras(indice_vista);
    if (m == 0) return; //No deberia pasar, pero por las dudas. Toca como con las mujeres, toca ir precavido
    //Fila 2, Hora de la muestra
    lcd_set_cursor(2, 1);
    lcd_printf("Hora: %02d:%02d:%02d   ", m->Hora.horas, m->Hora.minutos, m->Hora.segundos);

    //Fila 3, Temperatura
    lcd_set_cursor(3, 1);
    if (m->lec_ok) {
        lcd_printf("Temp: %d %%   ", m->Temperatura);
    } else {
        lcd_printf("Temp: --- C Error so puto   ");
    }
}

//Pantalla de muestras de LDR

static void dibujar_ldr(void)
{
    Muestra *m;
    uint8_t pct_luz;

    lcd_set_cursor(1, 1);
    if (numero_muestras == 0) {
        lcd_printf("No hay muestras");
    }
    else {lcd_printf("LDR %d/%d [B]=menu ", indice_vista + 1, numero_muestras);}

    if (numero_muestras == 0)
    {
        lcd_set_cursor(2,1);
        lcd_printf("Aun no hay muestras");
        lcd_set_cursor(3,1);
        lcd_printf(" Esperar %d min ", intervalo);
        lcd_set_cursor(4,1);
        lcd_printf("  [BACK] = Volver   ");
        return;
    }

    m = obtener_muestras(indice_vista);
    if (m == 0) return; //No deberia pasar, pero por las dudas. Toca como con las mujeres, toca ir precavido
    //Fila 2, Hora de la muestra
    lcd_set_cursor(2, 1);
    lcd_printf("Hora: %02d:%02d:%02d   ", m->Hora.horas, m->Hora.minutos, m->Hora.segundos);

    //Fila 3, LDR
    lcd_set_cursor(3, 1);
    lcd_printf("ADC: %4d   ", m->LDR);

    //Fila 4, porcentaje de luz
    pct_luz = (m->LDR * 100) / 1023;
    lcd_set_cursor(4, 1);
    lcd_printf("Luz: %3d %%   ", pct_luz);
}


static void dibujar_ajuste_hora(void)
{
    lcd_set_cursor(1, 1);
    lcd_printf("  AJUSTAR HORA    ");

    lcd_set_cursor(2, 1);
    lcd_printf("Hora: %02d:%02d:%02d   ", h_temp, m_temp, s_temp);

    lcd_set_cursor(3, 1);
    switch (campo_ajuste) {
        case 0: lcd_printf("    ^^              "); break;   /* bajo las horas */
        case 1: lcd_printf("         ^^         "); break;  /* bajo los minutos */
        case 2: lcd_printf("              ^^    "); break;  /* bajo los segundos */
        default: break;
    }
    lcd_set_cursor(4, 1);
    lcd_printf("[^][v]=cambiar[S]=OK");
    
}


void  menu_init(void) 
{ 
    estado = EST_RELOJ;
    pantalla_sucia = 1;
    cursor_menu = 0;
    indice_vista = 0;
    seg_anterior = 0xFF;
}

void menu_update(void)
{
    uint8_t arr = arriba();
    uint8_t abj = abajo();
    uint8_t sel = seleccionar();
    uint8_t back = volver();

    switch (estado)
    {
        case EST_RELOJ:
            if (Hora_actual.segundos != seg_anterior) 
            {
                pantalla_sucia = 1;
                seg_anterior = Hora_actual.segundos;
            }
            if (sel)
            {
                cursor_menu = 0;
                cambiar_estado(EST_MENU);
            }
            if (pantalla_sucia && estado == EST_RELOJ) {
                dibujar_reloj();
                pantalla_sucia = 0;
            }
            break;
        case EST_MENU:
            if (arr)
            {
                if (cursor_menu > 0)
                {
                    cursor_menu--;  
                }
                else 
                {
                    cursor_menu = Max_menu - 1;
                }
                pantalla_sucia = 1;
            }
            if (abj)
            {
                cursor_menu = (cursor_menu + 1) % Max_menu;
                pantalla_sucia = 1;
            }
            if (sel)
            {
                switch (cursor_menu)
                {
                    case 0:
                        cambiar_estado(EST_VER_DHT);
                        break;
                    case 1:
                        cambiar_estado(EST_VER_LDR);
                        break;
                    case 2:
                        h_temp = Hora_actual.horas;
                        m_temp = Hora_actual.minutos;
                        s_temp = Hora_actual.segundos;
                        campo_ajuste = 0;
                        cambiar_estado(EST_AJUSTAR_HORA);
                        break;
                    default:
                        break;
                }
            }
            break;
        case EST_VER_DHT:
            if (arr && numero_muestras > 0) {
                /* Muestra anterior */
                if (indice_vista > 0) {
                    indice_vista--;
                } else {
                    indice_vista = numero_muestras - 1;   /* vuelta al final */
                }
                pantalla_sucia = 1;
            }
            if (abj && numero_muestras > 0) {
                /* Siguiente muestra */
                indice_vista = (indice_vista + 1) % numero_muestras;
                pantalla_sucia = 1;
            }
            if (back || sel) {
                cambiar_estado(EST_MENU);
            }
            if (pantalla_sucia && estado == EST_VER_DHT) {
                dibujar_dht();
                pantalla_sucia = 0;
            }
            break;

        case EST_VER_LDR:
            if (arr && numero_muestras > 0) {
                if (indice_vista > 0) {
                    indice_vista--;
                } else {
                    indice_vista = numero_muestras - 1;
                }
                pantalla_sucia = 1;
            }
            if (abj && numero_muestras > 0) {
                indice_vista = (indice_vista + 1) % numero_muestras;
                pantalla_sucia = 1;
            }
            if (back || sel) {
                cambiar_estado(EST_MENU);
            }
            if (pantalla_sucia && estado == EST_VER_LDR) {
                dibujar_ldr();
                pantalla_sucia = 0;
            }
            break;

        case EST_AJUSTAR_HORA:
            if (arr) {
                /* Incrementar el campo activo */
                switch (campo_ajuste) {
                    case 0: h_temp = (h_temp + 1) % 24; break;
                    case 1: m_temp = (m_temp + 1) % 60; break;
                    case 2: s_temp = (s_temp + 1) % 60; break;
                    default: break;
                }
                pantalla_sucia = 1;
            }
            if (abj) {
                /* Decrementar el campo activo (con vuelta al maximo) */
                switch (campo_ajuste) {
                    case 0: h_temp = (h_temp == 0) ? 23 : h_temp - 1; break;
                    case 1: m_temp = (m_temp == 0) ? 59 : m_temp - 1; break;
                    case 2: s_temp = (s_temp == 0) ? 59 : s_temp - 1; break;
                    default: break;
                }
                pantalla_sucia = 1;
            }
            if (sel) {
                campo_ajuste++;
                if (campo_ajuste > 2) {
                    /* Se terminaron los 3 campos: guardar y volver al reloj */
                    Hora_actual.horas = h_temp;
                    Hora_actual.minutos = m_temp;
                    Hora_actual.segundos = s_temp;
                    seg_anterior = 0xFF;   /* forzar redibujo del reloj */
                    cambiar_estado(EST_RELOJ);
                } else {
                    pantalla_sucia = 1;   /* pasar al siguiente campo */
                }
            }
            if (back) {
                /* Cancelar sin guardar */
                cambiar_estado(EST_MENU);
            }
            if (pantalla_sucia && estado == EST_AJUSTAR_HORA) {
                dibujar_ajuste_hora();
                pantalla_sucia = 0;
            }
            break;

        default:
            cambiar_estado(EST_RELOJ);
            break;
    }
}