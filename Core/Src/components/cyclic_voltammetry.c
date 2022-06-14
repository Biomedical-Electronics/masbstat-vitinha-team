/*
 * cyclic_voltammetry.c
 *
 *  Created on: 3 may. 2022
 *      Author: eduardruiz
 */
// components and necessary includes
#include "components/cyclic_voltammetry.h"
#include "components/masb_comm_s.h"
#include "components/mcp4725_driver.h"
#include "components/formulas.h"
#include "components/chronoamperometry.h"
#include "main.h"

//includes of external peripherals UART, ADC, DAC and Timer 3
extern UART_HandleTypeDef huart2;
extern ADC_HandleTypeDef hadc1;
extern MCP4725_Handle_T hdac;
extern TIM_HandleTypeDef htim3;

extern int estadoMEDIDA;// timer 3 ISR controlled variable located in chronoamperometry file

uint16_t cumsum=0; //cumulated measurements all along cycles

//this function takes n ADC samples taking into account the voltage step, the increment
//directions, the voltage variation rate and the cyclic voltages of the cyclic voltammetry
//V1: voltage of begin, V2: objective voltage, Period and A_step: voltage step
void increment_sense(double V1,double V2,  uint32_t period, double A_step){
	struct Data_S sendPackage; //a structure to send the data to the PC is initialized
	double outV=V1;//first voltage
	uint16_t cont=0; //counter of the samples taken (each sample includes the voltage and the intensity)
	if (V1>V2){ //if the initial voltage is bigger than the final, decrements are done
		while (cont<(V1-V2)/A_step){
			while (estadoMEDIDA==1){//when the interruption activates, the measure is taken
				//ADC_Select_CH0();
				//measure of the voltage in the ADC
				HAL_ADC_Start(&hadc1);
				HAL_ADC_PollForConversion(&hadc1, 100);
				uint32_t IADC = HAL_ADC_GetValue(&hadc1);
				//measure of the intensity of the cell in the ADC
				HAL_ADC_Start(&hadc1);
				HAL_ADC_PollForConversion(&hadc1, 100);
				uint32_t VADC = HAL_ADC_GetValue(&hadc1);
				// Voltage and intensity of the cells are calculated with the ADC values
				double VREF = calculateVrefVoltage(VADC);
				double Icell = calculateIcellCurrent(IADC);
				//the data are saved in the send data structures and sent to the PC
				sendPackage.current = Icell;
				sendPackage.voltage = VREF;
				sendPackage.timeMs = cumsum * period;
				sendPackage.point = cumsum;
				MASB_COMM_S_sendData(sendPackage);
				// the total samples variables is incremented, and also the counter of samples
				cumsum++;
				cont++;
				//the voltage to the plate is decremented in an step and sent
				outV = outV - A_step;
				MCP4725_SetOutputVoltage(hdac, calculateDacOutputVoltage(outV));
				estadoMEDIDA = 0;	//the ISR variable to measure is deactivated
		}
	}
	}
	if (V1<V2){ //same than above but with increments because the objective is bigger than initial
		while (cont<(V2-V1)/A_step){
			while (estadoMEDIDA==1){

				HAL_ADC_Start(&hadc1);
				HAL_ADC_PollForConversion(&hadc1, 100);
				uint32_t IADC = HAL_ADC_GetValue(&hadc1);

				HAL_ADC_Start(&hadc1);
				HAL_ADC_PollForConversion(&hadc1, 100);
				uint32_t VADC = HAL_ADC_GetValue(&hadc1);


				double VREF = calculateVrefVoltage(VADC);
				double Icell = calculateIcellCurrent(IADC);

				sendPackage.current = Icell;
				sendPackage.voltage = VREF;
				sendPackage.timeMs = cumsum * period;
				sendPackage.point = cumsum;
				MASB_COMM_S_sendData(sendPackage);
				cumsum++;
				cont++;
				outV = outV + A_step;
				MCP4725_SetOutputVoltage(hdac, calculateDacOutputVoltage(outV)); // NUEVA TENSION
				estadoMEDIDA = 0;
			}
		}
	}
}

// Cyclic voltammetry function
void Cyclic_Voltammetry(struct CV_Configuration_S cvConfiguration){
	// the voltages of the CV, the number of cycles, the scanning rate and the
	// voltage step are received trough UART COM. The period between measures is calculated.
	double V_init=cvConfiguration.eBegin;
	double V_1=cvConfiguration.eVertex1;
	double V_2=cvConfiguration.eVertex2;
	uint8_t n_cycles=cvConfiguration.cycles;
	double ScanRate=cvConfiguration.scanRate;
	double A_step=cvConfiguration.eStep;
	uint32_t periodms=A_step/ScanRate*1000;
	//the initial voltage is set in the plate
	MCP4725_SetOutputVoltage(hdac, calculateDacOutputVoltage(V_init));
	// The timer period is adjusted, counter is set to 0 and interrups activated
	__HAL_TIM_SET_AUTORELOAD(&htim3,10*periodms);
	__HAL_TIM_SET_COUNTER(&htim3,0);
	HAL_TIM_Base_Start_IT(&htim3);
	// The realy is closed and the global variable of total samples is set to 0
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET);
	HAL_Delay(500);
	cumsum=0;
	//main loop, it executes the cyclic voltammetry a determined number of cycles
	for (int n=0;n<n_cycles;n++){
		// A cycle consists in going from the initial voltage to a vertex, then, to the
		//other vertex and, finally, to the beginning voltage again
		increment_sense(V_init,V_1,periodms,A_step);
		increment_sense(V_1,V_2,periodms,A_step);
		increment_sense(V_2,V_init,periodms,A_step);
	}
	// When finished, interrupts are closed and the relay opened
	HAL_TIM_Base_Stop_IT(&htim3);
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET);
}




