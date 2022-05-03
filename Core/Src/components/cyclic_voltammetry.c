/*
 * cyclic_voltammetry.c
 *
 *  Created on: 3 may. 2022
 *      Author: eduardruiz
 */

#include "components/cyclic_voltammetry.h"

uint32_t VrefADC;
uint32_t IADC; // adc value from Icell
double Icell;

struct IV_exe {
	double I[100];
	double V[100];
	uint16_t nsamples;
};

void Cyclic_Voltammetry(struct CV_Configuration_S cvConfiguration){
	double V_init=cvConfiguration.eBegin;
	double V_1=cvConfiguration.eVertex1;
	double V_2=cvConfiguration.eVertex2;
	uint8_t n_cycles=cvConfiguration.cycles;
	double ScanRate=cvConfiguration.scanRate;
	double A_step=cvConfiguration.eStep;
	uint16_t size_arrays=sizeOfArray(V_init,V_1,V_2,A_step,n_cycles);
	uint32_t periodms=A_step/ScanRate;
	double I_array[size_arrays];
	double V_array[size_arrays];
	MCP4725_SetOutputVoltage(hdac, calculateDacOutputVoltage(OutV)); // NUEVA TENSION
	struct IV_exe IV;
	uint16_t cum_measures=0;
	for (int n=0;n<n_cycles;n++){
		IV=increment_sense(V_init,V_1);
		for (int i=0; i<IV.nsamples; i++){
			I_array[cum_measures+i]=IV.I[i];
			V_array[cum_measures+i]=IV.V[i];
		}
		cum_measures+=IV.nsamples;
		IV=increment_sense(V_1,V_2);
		for (int i=0; i<IV.nsamples; i++){
			I_array[cum_measures+i]=IV.I[i];
			V_array[cum_measures+i]=IV.V[i];
		}
		cum_measures+=IV.nsamples;
		IV=increment_sense(V_2,V_init);
		for (int i=0;i<IV.nsamples; i++){
			I_array[cum_measures+i]=IV.I[i];
			V_array[cum_measures+i]=IV.V[i];
		}
		cum_measures+=IV.nsamples;
	}
	return I_array,V_array;

}

uint16_t sizeOfArray(double V0, double V1, double  V2,  double Vstep, uint8_t cycles){
	uint16_t size=ceil((fabs(V1-V0)/Vstep+fabs(V2-V1)/Vstep+fabs(V0-V2)/Vstep)*cycles);
	return size
}

struct IV_exe increment_sense(double V1,double V2, double A_step,&adc1){
	struct IV_exe IVstructure;
	if (V1>V2){
		double outV=V1;
		for (int i=0;i<(V1-V2)/A_step;i++){
			MCP4725_SetOutputVoltage(hdac,calculateDacOutputVoltage(outV)); // NUEVA TENSION
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
			I_array[i]=Icell;
			V_array[i]=VrefADC;
			outV=outV-A_step;
		}
	}
	if (V1<V2){
		double outV=V1;
		for (int i=0;i<(V2-V1)/A_step;i++){
			MCP4725_SetOutputVoltage(hdac,calculateDacOutputVoltage(outV)); // NUEVA TENSION
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
			I_array[i]=Icell;
			V_array[i]=VrefADC;
			outV=outV+A_step;
		}
	}
	IV_structure.I=I_array;
	IV_structure.V=V_array;
	IV_structure.nsamples=i;
	return IV_structure;
};
