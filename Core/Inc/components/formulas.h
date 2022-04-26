#ifndef INC_COMPONENTS_FORMULAS_H_
#define INC_COMPONENTS_FORMULAS_H_

#include <stdint.h>

// Se le pasa el valor obtenido para HAL_ADC_getValue para el canal VCELL
// y devuelve la tension entre WE y RE
double calculateVrefVoltage(uint32_t adcValue);

// Se le pasa el valor obtenido para HAL_ADC_getValue para el canal ICELL
// y devuelve la corriente de la celda
double calculateIcellCurrent(uint32_t adcValue);

// Se le pasa el valor de tension deseado entre WE y RE y devuelve el valor
// que hay que darle a la funcion MCP4725_SetOutputVoltage
float calculateDacOutputVoltage(double desiredVrefVoltage);

#endif /* INC_COMPONENTS_STM32F4XX_ADC_H_ */
