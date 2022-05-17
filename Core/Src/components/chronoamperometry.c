/*
 * chronoamperometry.c
 *
 *  Created on: Apr 26, 2022
 *      Author: eduardruiz
 */

#include "components/masb_comm_s.h"

uint32_t VrefADC;
uint32_t IADC; // adc value from Icell
double Icell;
extern &htim2;

void ChronoAmperometry(struct CA_Configuration_S caConfiguration){


	double OutV=caConfiguration.eDC;
	uint32_t periodms=caConfiguration.samplingPeriodMs;
	uint32_t duration=caConfiguration.measurementTime;
	uint16_t n_samples=duration/periodms;
	struct Data_S sendPackage;
	MCP4725_SetOutputVoltage(hdac,calculateDacOutputVoltage(OutV)); // NUEVA TENSION
	TIM_TimeBaseInitStructure.TIM_Prescaler = 8399+1;
	TIM_TimeBaseInitStructure.TIM_Period = periodms*10000;
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_ADC_Start(&hadc1); // iniciamos la conversion para el divisor de tension
	HAL_ADC_PollForConversion(&hadc1, 200); // esperamos que finalice la conversion
	    							// si en 200 ms no termina, salimos
	VrefADC = HAL_ADC_GetValue(&hadc1);
	int i=0;
	while (i<n_samples) {
		if (estadoMEDIDA==1){
		HAL_ADC_Start(&hadc1); // iniciamos la conversion para la intensidad de la celda
		HAL_ADC_PollForConversion(&hadc1, 200); // esperamos que finalice la conversion
																							// si en 200 ms no termina, salimos
		IADC = HAL_ADC_GetValue(&hadc1);  // leemos el resultado de la conversion y lo
												 // guardamos en tempADC
		Icell=calculateIcellCurrent(IADC);

		sendPackage.current = Icell;
		sendPackage.timeMs = i*periodms;
		sendPackage.voltage = OutV;
		sendPackage.point = i;

		MASB_COMM_S_sendData(sendPackage);
		i++;
		estadoMEDIDA=0;
		}
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {

	estadoMEDIDA = 1;

}

<<<<<<< HEAD

=======
>>>>>>> 0c692f52700943d0b67f21d635382bee88c56621
