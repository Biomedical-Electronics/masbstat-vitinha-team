/*
 * chronoamperometry.c
 *
 *  Created on: Apr 26, 2022
 *      Author: eduardruiz
 */

uint32_t VrefADC;
uint32_t IADC; // adc value from Icell
uint32_t Icell;

void ChronoAmperometry(struct CA_Configuration_S caConfiguration){

	uint32_t OutV=caConfiguration.eDC;
	MCP4725_SetOutputVoltage(hdac,calculateDacOutputVoltage(OutV)); // NUEVA TENSION
	HAL_ADC_Start(&hadc1); // iniciamos la conversion para el divisor de tension
	HAL_ADC_PollForConversion(&hadc1, 200); // esperamos que finalice la conversion
	    							// si en 200 ms no termina, salimos
	VrefADC = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Start(&hadc1); // iniciamos la conversion para la intensidad de la celda
	HAL_ADC_PollForConversion(&hadc1, 200); // esperamos que finalice la conversion
	    																				// si en 200 ms no termina, salimos
	IADC = HAL_ADC_GetValue(&hadc1);  // leemos el resultado de la conversion y lo
	                                         // guardamos en tempADC
	Icell=calculateIcellCurrent(IADC);

	return IADC;

}
