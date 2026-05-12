/* 
    @author: Trivi
*/

#include "sensores.h"
#include "../lib/gpio.h"
#include "../lib/adc.h"

void sensores_init(void){
    //Inicializa el ADC usando la nueva abstraccion HAL_ADC
    hal_adc_config_t adc_cfg;
    adc_cfg.reference = HAL_ADC_REF_AVCC;
    adc_cfg.prescaler = HAL_ADC_PRESCALER_128; //125KHz para 16MHz
    adc_cfg.vref_mv = 5000;
    HAL_ADC_Init(&adc_cfg);
}

uint16_t LDR_read(void){
    uint16_t valor_adc = 0;
    //Lee el canal 2, donde esta el LDR (segun define Canal_LDR 2)
    HAL_ADC_Read(Canal_LDR, &valor_adc);
    return valor_adc;
}

static uint8_t esperar_estado(uint8_t estado, uint8_t max_us) {
    uint8_t count = 0;
    while (HAL_GPIO_ReadPin(puerto_sensores, sensor_temp) != estado) {
        if (++count > max_us) return 0;
        _delay_us(1);
    }
    return 1;
}

uint8_t temp_read(uint8_t *temp, uint8_t *humidity){
    uint8_t datos[5] = {0};

    // 1. Inicio: Pulso bajo del master por 18ms
    HAL_GPIO_ConfigPin(puerto_sensores, sensor_temp, OUTPUT);
    HAL_GPIO_WritePin(puerto_sensores, sensor_temp, LOW);
    _delay_ms(18);

    // 2. Soltar línea y esperar respuesta del sensor
    HAL_GPIO_ConfigPin(puerto_sensores, sensor_temp, INPUT);
    HAL_GPIO_SetPullup(puerto_sensores, sensor_temp, 0); // Asumiendo pull-up externa
    _delay_us(40);

    // 3. Confirmación del sensor
    if (!esperar_estado(HIGH, 80)) return 0;
    if (!esperar_estado(LOW, 80)) return 0;

    // 4. Leer 40 bits (5 bytes)
    for (uint8_t i = 0; i < 5; i++) {
        for (int8_t j = 7; j >= 0; j--) {
            if (!esperar_estado(HIGH, 60)) return 0;
            _delay_us(35); // Esperar el umbral (0 es ~28us, 1 es ~70us)
            
            if (HAL_GPIO_ReadPin(puerto_sensores, sensor_temp)) {
                datos[i] |= (1 << j);
            }
            if (!esperar_estado(LOW, 60)) return 0;
        }
    }

    // 5. Validar Checksum y asignar
    if ((uint8_t)(datos[0] + datos[1] + datos[2] + datos[3]) != datos[4]) {
        return 0; // Error de checksum
    }

    *humidity = datos[0];
    *temp = datos[2];
    
    return 1; // Lectura exitosa
}