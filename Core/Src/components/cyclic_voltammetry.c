/*
 * cyclic_voltammetry.c
 *
 *  Created on: 3 may. 2022
 *      Author: eduardruiz
 */

#include "components/cyclic_voltammetry.h"
#include "components/masb_comm_s.h"
extern ADC_HandleTypeDef hadc1;

double VrefADC;
uint32_t IADC; // adc value from Icell
double Icell;

struct Data_S{
	uint32_t point;
	uint32_t timeMs;
	double voltage;
	double current;
};



void Cyclic_Voltammetry(struct CV_Configuration_S cvConfiguration){
	double V_init=cvConfiguration.eBegin;
	double V_1=cvConfiguration.eVertex1;
	double V_2=cvConfiguration.eVertex2;
	uint8_t n_cycles=cvConfiguration.cycles;
	double ScanRate=cvConfiguration.scanRate;
	double A_step=cvConfiguration.eStep;
	uint32_t periodms=A_step/ScanRate;
	MCP4725_SetOutputVoltage(hdac, calculateDacOutputVoltage(OutV)); // NUEVA TENSION
	uint16_t l1=1;
	uint16_t cumsum=0;
	for (int n=0;n<n_cycles;n++){
		l1=increment_sense(V_init,V_1,cumsum,periodms,A_step);
		cumsum=cumsum+l1+1;
		l1=increment_sense(V_1,V_2,cumsum,periodms,A_step);
		cumsum=cumsum+l1+1;
		l1=increment_sense(V_2,V_init,cumsum,periodms,A_step);
		cumsum=cumsum+l1+1;
	}
}

uint16_t increment_sense(double V1,double V2, uint16_t n, uint32_t period, double A_step){
	struct Data_S sendPackage;
	double outV=V1;
	uint16_t i=0;
	if (V1>V2){
		for(i=0;i<(V1-V2)/A_step;i++){

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

			outV=outV-A_step;
			MCP4725_SetOutputVoltage(&hdac,calculateDacOutputVoltage(outV)); // NUEVA TENSION
			sendPackage.current=Icell;
			sendPackage.voltage=VrefADC;
			sendPackage.time=(n+i)*period;
			sendPackage.point=n+i;
			MASB_COMM_S_sendData(sendPackage);
		}
	}
	if (V1<V2){
		for (i=0;i<(V2-V1)/A_step;i++){
			HAL_ADC_Start(&hadc1); // iniciamos la conversion para el divisor de tension
			HAL_ADC_PollForConversion(&hadc1, 200); // esperamos que finalice la conversion
			// si en 200 ms no termina, salimos
			VrefADC = HAL_ADC_GetValue(&hadc1);
			HAL_ADC_Start(&hadc1); // iniciamos la conversion para la intensidad de la celda
			HAL_ADC_PollForConversion(&hadc1, 200); // esperamos que finalice la conversion
													// si en 200 ms no termina, salimos
			IADC = HAL_ADC_GetValue(&hadc1); // leemos el resultado de la conversion y lo
											 // guardamos en tempADC
			Icell = calculateIcellCurrent(IADC);
			outV = outV + A_step;
			MCP4725_SetOutputVoltage(hdac, calculateDacOutputVoltage(outV)); // NUEVA TENSION
			sendPackage.current=Icell;
			sendPackage.voltage=VrefADC;
			sendPackage.time=(n+i)*period;
			sendPackage.point=n+i;
			MASB_COMM_S_sendData(sendPackage);
		}
	}
	return i;
}
