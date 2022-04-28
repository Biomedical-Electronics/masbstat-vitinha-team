/*
 * chronoamperometry.c
 *
 *  Created on: Apr 26, 2022
 *      Author: eduardruiz
 */

uint32_t VrefADC;
uint32_t IADC; // adc value from Icell
double Icell;
double I_array[]={};

void ChronoAmperometry(struct CA_Configuration_S caConfiguration){

	double OutV=caConfiguration.eDC;
	uint32_t periodms=caConfiguration.samplingPeriodMs;
	uint32_t duration=caConfiguration.measurementTime;
	uint16_t n_samples=duration/periodms;
	MCP4725_SetOutputVoltage(hdac,calculateDacOutputVoltage(OutV)); // NUEVA TENSION
	HAL_ADC_Start(&hadc1); // iniciamos la conversion para el divisor de tension
	HAL_ADC_PollForConversion(&hadc1, 200); // esperamos que finalice la conversion
	    							// si en 200 ms no termina, salimos
	VrefADC = HAL_ADC_GetValue(&hadc1);

	for (i=0;i<n_samples;i++){
		HAL_ADC_Start(&hadc1); // iniciamos la conversion para la intensidad de la celda
		HAL_ADC_PollForConversion(&hadc1, 200); // esperamos que finalice la conversion
																							// si en 200 ms no termina, salimos
		IADC = HAL_ADC_GetValue(&hadc1);  // leemos el resultado de la conversion y lo
												 // guardamos en tempADC
		Icell=calculateIcellCurrent(IADC);
		I_array[i]=Icell;
	}
	return Icell;

}
