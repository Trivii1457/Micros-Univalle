#include "menu.h"
#include "botones.h"
#include "reloj.h"
#include "../lib/LCD.h"
#include "../lib/gpio.h"
#include <stdio.h>
#include "big_font.h"

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

static uint8_t h_temp, m_temp; 
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

static void dibujar_digito_grande(uint8_t digito, uint8_t offset_col) {
    if (digito > 9) return;
    for (uint8_t r = 0; r < 4; r++) { // 4 filas de alto
        lcd_set_cursor(r + 1, offset_col); // El offset_col es 1-based (del 1 al 20)
        for (uint8_t c = 0; c < 3; c++) { 
            lcd_write(big_digits[digito][r][c]);
        }
    }
}

static void dibujar_reloj(void)
{
   

    uint8_t h1 = Hora_actual.horas / 10;
    uint8_t h2 = Hora_actual.horas % 10;
    uint8_t m1 = Hora_actual.minutos / 10;
    uint8_t m2 = Hora_actual.minutos % 10;

    lcd_clear();

    // Dibujar los 4 digitos
    dibujar_digito_grande(h1, 3);
    dibujar_digito_grande(h2, 7);
    dibujar_digito_grande(m1, 12);
    dibujar_digito_grande(m2, 16);

    // Dibujar los dos puntos separadores en el centro (columnas 10 y 11, filas 2 y 3)
    lcd_set_cursor(2, 10); lcd_write(' '); lcd_write((uint8_t)4); // Bloque como pto superior
    lcd_set_cursor(3, 10); lcd_write(' '); lcd_write((uint8_t)4); // Bloque como pto inferior
}

static void dibujar_menu(void)
{ 
    lcd_set_cursor(1, 1);
    lcd_puts("   MENU PRINCIPAL   ");
    for (uint8_t i = 0; i < Max_menu; i++) {
        lcd_set_cursor(2 + i, 1);
        if (i == cursor_menu) {
            lcd_puts("> ");
        } else {
            lcd_puts("  ");
        }
        lcd_puts((char*)opciones_menu[i]);
    }
}

static void dibujar_dht(void)
{
    Muestra *m;

    lcd_set_cursor(1, 1);
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
    lcd_printf("Hora: %02d:%02d      ", m->Hora.horas, m->Hora.minutos);

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
    lcd_printf("Hora: %02d:%02d      ", m->Hora.horas, m->Hora.minutos);

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
    lcd_printf("Hora: %02d:%02d      ", h_temp, m_temp);

    lcd_set_cursor(3, 1);
    switch (campo_ajuste) {
        case 0: lcd_printf("    ^^              "); break;   /* bajo las horas */
        case 1: lcd_printf("         ^^         "); break;  /* bajo los minutos */
        
        default: break;
    }
    lcd_set_cursor(4, 1);
    lcd_printf("[^][v]=cambiar[S]=OK");
    
}


void  menu_init(void) 
{ 
    // Cargar los 8 caracteres personalizados en la CGRAM del LCD
    lcd_custom_char(0, (uint8_t *)bf_top_bar);
    lcd_custom_char(1, (uint8_t *)bf_bot_bar);
    lcd_custom_char(2, (uint8_t *)bf_top_right);
    lcd_custom_char(3, (uint8_t *)bf_top_left);
    lcd_custom_char(4, (uint8_t *)bf_solid);
    lcd_custom_char(5, (uint8_t *)bf_mid_bot);
    lcd_custom_char(6, (uint8_t *)bf_bot_right);
    lcd_custom_char(7, (uint8_t *)bf_bot_left);

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
                        
                        campo_ajuste = 0;
                        cambiar_estado(EST_AJUSTAR_HORA);
                        break;
                    default:
                        break;
                }
            }
            if (back)
            {
                cambiar_estado(EST_RELOJ);
            }
            if (pantalla_sucia && estado == EST_MENU) {
                dibujar_menu();
                pantalla_sucia = 0;
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
                    
                    default: break;
                }
                pantalla_sucia = 1;
            }
            if (abj) {
                /* Decrementar el campo activo (con vuelta al maximo) */
                switch (campo_ajuste) {
                    case 0: h_temp = (h_temp == 0) ? 23 : h_temp - 1; break;
                    case 1: m_temp = (m_temp == 0) ? 59 : m_temp - 1; break;
                    
                    default: break;
                }
                pantalla_sucia = 1;
            }
            if (sel) {
                campo_ajuste++;
                if (campo_ajuste > 1) {
                    /* Se terminaron los 3 campos: guardar y volver al reloj */
                    Hora_actual.horas = h_temp;
                    Hora_actual.minutos = m_temp;
                    Hora_actual.segundos = 0;
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
