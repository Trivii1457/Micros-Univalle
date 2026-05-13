/* 
    @author: Trivi
*/

#include "sensores.h"
#include "../lib/gpio.h"
#include "../lib/adc.h"

void sensores_init(void){
    //Inicializa el ADC
    hal_adc_config_t adc_cfg = {0}; // Inicializar todo a 0
    adc_cfg.reference = HAL_ADC_REF_AVCC;
    adc_cfg.prescaler = HAL_ADC_PRESCALER_128; //125KHz para 16MHz
    adc_cfg.use_irq = 0; // Usar polling (muy importante para evitar errores)
    adc_cfg.vref_mv = 5000;
    HAL_ADC_Init(&adc_cfg);
}

uint16_t LDR_read(void){
    uint16_t valor_adc = 0;
    //Lee el canal 2, donde esta el LDR (segun define Canal_LDR 2)
    HAL_ADC_Read(Canal_LDR,  &valor_adc); 
    return valor_adc;
}



uint16_t HW870_read(void){
    uint16_t valor_adc = 0;
    HAL_ADC_Read(Canal_HW870,  &valor_adc);
    return valor_adc;
}