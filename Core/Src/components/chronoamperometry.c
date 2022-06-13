/*
 * chronoamperometry.c
 *
 *  Created on: Apr 26, 2022
 *      Author: Eduard and Elliot
 */

//Includes of the needed components

#include "components/masb_comm_s.h"
#include "components/mcp4725_driver.h"
#include "components/formulas.h"
#include "main.h"


extern ADC_HandleTypeDef hadc1; //variable of the hdac imported
extern TIM_HandleTypeDef htim3; //variable of the timer
MCP4725_Handle_T hdac; //variable of the dac


int estadoMEDIDA=0; //this variable changes with the timer interrupts to allow the ADC measures

void ChronoAmperometry(struct CA_Configuration_S caConfiguration){
	//voltage for the chrono, period between measures, the total duration are received by UART
	//COM and the number of samples is calculated
	double OutV=caConfiguration.eDC;
	uint32_t periodms=caConfiguration.samplingPeriodMs;
	uint32_t duration=caConfiguration.measurementTime;
	uint32_t n_samples=duration*1000/periodms + 1;
	// The new voltage is sent to the plate, a delay is waited and the relay closed
	MCP4725_SetOutputVoltage(hdac,calculateDacOutputVoltage(OutV));
	HAL_Delay(500);
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET);
	// The timer period is adjusted with the period received by USART, the counter to 0 and
	//interrupts are activated
	__HAL_TIM_SET_AUTORELOAD(&htim3,10*periodms);
	__HAL_TIM_SET_COUNTER(&htim3,0);
	HAL_TIM_Base_Start_IT(&htim3);
	//a loop is initialized to obtain n samples with the ADC
	int i=0;
	while (i<n_samples) {
		while (estadoMEDIDA==1){

				HAL_ADC_Start(&hadc1);
				HAL_ADC_PollForConversion(&hadc1, 100);
				uint32_t IADC = HAL_ADC_GetValue(&hadc1); //measure of channel 0

				HAL_ADC_Start(&hadc1);
				HAL_ADC_PollForConversion(&hadc1, 100);
				uint32_t VADC = HAL_ADC_GetValue(&hadc1);//measure of channel 1

				double VREF = calculateVrefVoltage(VADC); //Current of the cell is calculated
				double Icell=calculateIcellCurrent(IADC);
				//double Icell=(VADC); //Voltage of reference is calculated
				// A structure is created to send the data and they are sent to the PC
				struct Data_S sendPackage;
				sendPackage.current =Icell;
				sendPackage.timeMs = i * periodms;
				sendPackage.voltage = VREF;
				sendPackage.point = i;
				MASB_COMM_S_sendData(sendPackage);
				i++; //the counter is incremented
				estadoMEDIDA=0; //the measure state is change to deactivated
		}


	}
	//when finished, interrupts and relay deactivated
	HAL_TIM_Base_Stop_IT(&htim3);
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET);
}

//ISR function of the interruptions

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim3){
	estadoMEDIDA=1;
}


