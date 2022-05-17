/*
 * cyclic_voltammetry.c
 *
 *  Created on: 3 may. 2022
 *      Author: eduardruiz
 */

#include "components/cyclic_voltammetry.h"
#include "components/masb_comm_s.h"
#include "components/mcp4725_driver.h"
#include "components/formulas.h"

extern ADC_HandleTypeDef hadc1;
extern MCP4725_Handle_T hdac;
double VrefADC;
uint32_t IADC; // adc value from Icell
double Icell;
extern ADC_HandleTypeDef hadc1;
int estadoMEDIDA=0;
extern TIM_HandleTypeDef htim2;
uint16_t cumsum=0;

void increment_sense(double V1,double V2,  uint32_t period, double A_step){
	struct Data_S sendPackage;
	double outV=V1;
	uint16_t cont=0;
	if (V1>V2){
		while (cont<(V1-V2)/A_step){
			if (estadoMEDIDA==1){

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
			MCP4725_SetOutputVoltage(hdac,calculateDacOutputVoltage(outV)); // NUEVA TENSION
			sendPackage.current=Icell;
			sendPackage.voltage=VrefADC;
			sendPackage.timeMs=cumsum*period;
			sendPackage.point=cumsum;
			MASB_COMM_S_sendData(sendPackage);
			cumsum++;
			cont++;
			estadoMEDIDA=0;
		}
	}
	}
	if (V1<V2){
		while (cont<(V2-V1)/A_step){
			if (estadoMEDIDA==1){
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
			sendPackage.timeMs=cumsum*period;
			sendPackage.point=cumsum;
			MASB_COMM_S_sendData(sendPackage);
			cumsum++;
			cont++;
			estadoMEDIDA=0;
		}
	}
	}
}
void Cyclic_Voltammetry(struct CV_Configuration_S cvConfiguration){
	double V_init=cvConfiguration.eBegin;
	double V_1=cvConfiguration.eVertex1;
	double V_2=cvConfiguration.eVertex2;
	uint8_t n_cycles=cvConfiguration.cycles;
	double ScanRate=cvConfiguration.scanRate;
	double A_step=cvConfiguration.eStep;
	uint32_t periodms=A_step/ScanRate;
	htim2.Init.Prescaler = 8399;
	htim2.Init.Period = 10*periodms;
	MCP4725_SetOutputVoltage(hdac, calculateDacOutputVoltage(V_init)); // NUEVA TENSION
	cumsum=0;
	for (int n=0;n<n_cycles;n++){
		increment_sense(V_init,V_1,periodms,A_step);
		increment_sense(V_1,V_2,periodms,A_step);
		increment_sense(V_2,V_init,periodms,A_step);
	}
}



void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim2) {

	estadoMEDIDA = 1;

}

