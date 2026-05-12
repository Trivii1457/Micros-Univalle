#ifndef BIG_FONT_H
#define BIG_FONT_H

#include <stdint.h>

/* Mapas de caracteres personalizados (8 caracteres basicos)
   para construir numeros grandes en 4 filas */

// 0: Bar superior
static const uint8_t bf_top_bar[8] = {
    0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 1: Bar inferior
static const uint8_t bf_bot_bar[8] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F
};

// 2: Bloque medio/superior - curva derecha
static const uint8_t bf_top_right[8] = {
    0x1E, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F
};

// 3: Bloque medio/superior - curva izquierda
static const uint8_t bf_top_left[8] = {
    0x0F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F
};

// 4: Bloque completo (a veces el ASCII 255 es transparente, asi que aseguramos uno macizo)
static const uint8_t bf_solid[8] = {
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F
};

// 5: Bar medio bajo
static const uint8_t bf_mid_bot[8] = {
    0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x00, 0x00
};

// 6: Curva inferior derecha
static const uint8_t bf_bot_right[8] = {
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1E, 0x1C
};

// 7: Curva inferior izquierda
static const uint8_t bf_bot_left[8] = {
    0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x0F, 0x07
};

/*
    Mapeo de digitos:
    Cada digito mide 3 de ancho por 4 de alto.
    ' ' (32) = Espacio vacio.
    0-7 = Los custom chars.
*/

static const uint8_t big_digits[10][4][3] = {
    // 0
    {
        {3, 0, 2},
        {4, 32, 4},
        {4, 32, 4},
        {7, 1, 6}
    },
    // 1
    {
        {0, 2, 32},
        {32, 4, 32},
        {32, 4, 32},
        {1, 4, 1}
    },
    // 2
    {
        {0, 0, 2},
        {32, 32, 4},
        {3, 0, 6},
        {4, 1, 1}
    },
    // 3
    {
        {0, 0, 2},
        {32, 5, 4},
        {32, 32, 4},
        {1, 1, 6}
    },
    // 4
    {
        {4, 32, 4},
        {4, 5, 4},
        {32, 32, 4},
        {32, 32, 4}
    },
    // 5
    {
        {3, 0, 0},
        {7, 5, 32},
        {32, 32, 4},
        {1, 1, 6}
    },
    // 6
    {
        {3, 0, 0},
        {4, 5, 32},
        {4, 32, 4},
        {7, 1, 6}
    },
    // 7
    {
        {0, 0, 2},
        {32, 32, 4},
        {32, 32, 4},
        {32, 32, 4}
    },
    // 8
    {
        {3, 0, 2},
        {7, 5, 6},
        {4, 32, 4},
        {7, 1, 6}
    },
    // 9
    {
        {3, 0, 2},
        {4, 32, 4},
        {32, 5, 6},
        {32, 32, 4}
    }
};

#endif