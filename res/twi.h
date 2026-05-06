/*
 * twi.h
 *
 * Driver basico de TWI (I2C) para el ATmega1284P.
 * TWI = Two Wire Interface, el nombre que AVR le da al protocolo I2C.
 *
 * Solo implementa modo maestro (master), que es lo que necesitamos
 * para hablar con el modulo PCF8574 de la LCD.
 *
 * Pines TWI del ATmega1284P:
 *   PC0 -> SCL (reloj)
 *   PC1 -> SDA (datos)
 *
 * IMPORTANTE: Estos pines necesitan resistencias pull-up externas
 * de 4.7 kOhm hacia VCC. Sin ellas el bus no funciona.
 * El pull-up interno del AVR (~50 kOhm) es demasiado debil para I2C.
 *
 * Como funciona I2C en resumen:
 *   1. El maestro genera una condicion START (SDA baja mientras SCL alta)
 *   2. El maestro envia la direccion del esclavo (7 bits) + bit R/W
 *   3. El esclavo responde con un ACK (bit en bajo)
 *   4. Se intercambian bytes, cada uno confirmado con ACK
 *   5. El maestro genera STOP (SDA sube mientras SCL alta)
 *
 * Created: 2026
 * Author : Estudiante
 */

#ifndef TWI_H_
#define TWI_H_

#include <avr/io.h>
#include <stdint.h>

/* =====================================================
   Configuracion de velocidad
   ===================================================== */

/*
 * Velocidad del bus I2C: 100 kHz (modo estandar).
 * Formula del registro TWBR:
 *   TWBR = (F_CPU / (2 * F_SCL) - 8) / prescaler
 * Con F_CPU = 8MHz, F_SCL = 100kHz, prescaler = 1:
 *   TWBR = (8000000 / 200000 - 8) / 1 = 32
 */
#define TWI_TWBR_VALUE   32U
#define TWI_TWSR_PRESCALER  0x00U   /* prescaler = 1 (bits TWPS = 00) */

/* =====================================================
   Codigos de estado TWI (registro TWSR)
   ===================================================== */
#define TWI_START          0x08U   /* condicion START transmitida OK */
#define TWI_REP_START      0x10U   /* condicion START repetida OK */
#define TWI_MT_SLA_ACK     0x18U   /* direccion + WRITE enviada, ACK recibido */
#define TWI_MT_DATA_ACK    0x28U   /* byte de datos enviado, ACK recibido */

/* =====================================================
   Funciones publicas
   ===================================================== */

/*
 * TWI_Init()
 * Configura el periferico TWI del micro.
 * Llamar una vez en main() antes de usar la LCD.
 */
void TWI_Init(void);

/*
 * TWI_Start()
 * Envia una condicion START al bus.
 * Retorna 0 si fue exitoso, -1 si hubo error.
 */
int8_t TWI_Start(void);

/*
 * TWI_Stop()
 * Envia una condicion STOP al bus (libera el bus).
 */
void TWI_Stop(void);

/*
 * TWI_WriteByte(byte)
 * Envia un byte por el bus (puede ser la direccion o un dato).
 * Retorna 0 si el esclavo confirmo con ACK, -1 si hubo NACK o error.
 */
int8_t TWI_WriteByte(uint8_t byte);

/*
 * TWI_WriteToAddress(addr, data, len)
 * Funcion de alto nivel: envia un arreglo de bytes a un esclavo.
 * Maneja el START, la direccion, los datos y el STOP automaticamente.
 *
 * @param addr  Direccion I2C del esclavo (7 bits, sin el bit R/W)
 * @param data  Puntero al arreglo de bytes a enviar
 * @param len   Cantidad de bytes a enviar
 * @return  0 si todo fue bien, -1 si hubo algun error
 */
int8_t TWI_WriteToAddress(uint8_t addr, uint8_t *data, uint8_t len);

#endif /* TWI_H_ */