/*
 * stm32f4xx_adc.c
 *
 *  Created on: May 20, 2021
 *      Author: Albert
 */

#include "components/formulas.h"

const double u2b_m = 8.0 / 3.3;
const double u2b_b = 4.0;
const double rTia = 50e3;

double calculateVrefVoltage(uint32_t adcValue) {

    double adcVoltage = ((double)adcValue) / 4095.0 * 3.3;

    return -((adcVoltage * u2b_m) - u2b_b);
}

double calculateIcellCurrent(uint32_t adcValue) {

    double adcVoltage = ((double)adcValue) / 4095.0 * 3.3;

    return ((adcVoltage * u2b_m) - u2b_b) / rTia;

}

float calculateDacOutputVoltage(double desiredVrefVoltage) {

    return (float)(desiredVrefVoltage / 2.0 + 2.0);

}
