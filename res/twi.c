/*
 * twi.c
 *
 * Implementacion del driver TWI para el ATmega1284P.
 * Ver twi.h para descripcion general.
 *
 * Created: 2026
 * Author : Estudiante
 */

#include "twi.h"
#include <util/twi.h>   /* macros TW_STATUS, TW_START, etc. del avr-libc */

/* =====================================================
   Macro auxiliar interna
   ===================================================== */

/*
 * TWI_WAIT()
 * Espera a que el hardware TWI termine la operacion actual.
 * El bit TWINT se pone en 1 cuando el hardware termina.
 * Escribir 1 en TWINT lo limpia (es de tipo "write 1 to clear").
 */
#define TWI_WAIT()   while (!(TWCR & (1 << TWINT)))

/* =====================================================
   Implementacion
   ===================================================== */

void TWI_Init(void) {
    /*
     * Configurar velocidad del bus.
     * TWBR controla la frecuencia del reloj SCL.
     * Ver twi.h para la formula de calculo.
     */
    TWBR = TWI_TWBR_VALUE;
    TWSR = TWI_TWSR_PRESCALER;   /* prescaler = 1 */

    /*
     * Habilitar el modulo TWI (bit TWEN).
     * Los pines PC0 y PC1 quedan bajo control del periferico TWI.
     * No hace falta configurarlos manualmente con DDR.
     */
    TWCR = (1 << TWEN);
}

int8_t TWI_Start(void) {
    /*
     * Generar condicion START:
     *   TWINT = 1 -> limpiar la bandera de interrupcion (requisito para operar)
     *   TWSTA = 1 -> pedir que se genere la condicion START
     *   TWEN  = 1 -> mantener el modulo TWI habilitado
     */
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

    /* Esperar a que el hardware termine de enviar el START */
    TWI_WAIT();

    /*
     * Verificar el codigo de estado.
     * TW_STATUS es una macro de avr-libc que lee los 5 bits de estado de TWSR.
     * Debe ser 0x08 (START) o 0x10 (START repetido).
     */
    uint8_t status = TW_STATUS;
    if (status != TWI_START && status != TWI_REP_START) {
        return -1;   /* algo salio mal */
    }
    return 0;
}

void TWI_Stop(void) {
    /*
     * Generar condicion STOP:
     *   TWINT = 1 -> limpiar bandera
     *   TWSTO = 1 -> pedir condicion STOP
     *   TWEN  = 1 -> mantener modulo habilitado
     *
     * Despues del STOP el bus queda libre para otra transaccion.
     * No hay que esperar TWINT: el hardware de STOP no lo activa.
     */
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

int8_t TWI_WriteByte(uint8_t byte) {
    /* Cargar el byte en el registro de datos TWI */
    TWDR = byte;

    /*
     * Iniciar la transmision:
     *   TWINT = 1 -> limpiar bandera para que el hardware arranque
     *   TWEN  = 1 -> mantener TWI habilitado
     * (Sin TWSTA ni TWSTO: solo enviar el byte)
     */
    TWCR = (1 << TWINT) | (1 << TWEN);

    /* Esperar a que el byte se transmita completamente */
    TWI_WAIT();

    /*
     * Verificar que el esclavo respondio con ACK.
     * 0x18 = direccion enviada + ACK recibido
     * 0x28 = dato enviado + ACK recibido
     */
    uint8_t status = TW_STATUS;
    if (status != TWI_MT_SLA_ACK && status != TWI_MT_DATA_ACK) {
        return -1;   /* el esclavo no respondio (NACK) o hubo error */
    }
    return 0;
}

int8_t TWI_WriteToAddress(uint8_t addr, uint8_t *data, uint8_t len) {
    uint8_t i;

    /* Paso 1: generar START */
    if (TWI_Start() != 0) {
        TWI_Stop();
        return -1;
    }

    /*
     * Paso 2: enviar la direccion del esclavo.
     * La direccion ocupa los 7 bits altos, el bit 0 es R/W:
     *   0 = escritura (W), 1 = lectura (R)
     * Desplazamos 1 bit a la izquierda y ponemos 0 en el bit R/W.
     */
    if (TWI_WriteByte((uint8_t)(addr << 1)) != 0) {
        TWI_Stop();
        return -1;
    }

    /* Paso 3: enviar los bytes de datos */
    for (i = 0; i < len; i++) {
        if (TWI_WriteByte(data[i]) != 0) {
            TWI_Stop();
            return -1;
        }
    }

    /* Paso 4: liberar el bus */
    TWI_Stop();
    return 0;
}