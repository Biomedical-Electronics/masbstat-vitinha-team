/**
  ******************************************************************************
  * @file		chronoamperometry.c
  * @brief		Gestión de la cronoamperometría.
  * @author		Albert Álvarez Carulla
  * @copyright	Copyright 2020 Albert Álvarez Carulla. All rights reserved.
  ******************************************************************************
  */

// Anadimos los archivos header necesarios para el programa
#include "components/chronoamperometry.h" // donde tenemos definida la crono
#include "components/masb_comm_s.h" // en este header se detallan los paquetes de comandos que inician y realizan las mediciones
#include "components/dac.h"
#include "components/adc.h"
#include "components/stm32main.h"
#include "main.h"
#include "components/formulas.h"

// per configurar el voltatge de la cela

extern TIM_HandleTypeDef htim3; //timer
extern uint8_t count;
extern uint8_t state; 
uint32_t samplingPeriod;

// caConfiguration=MASB_COMM_S_getCaConfiguration(void)

void ChronoAmperometry(struct CA_Configuration_S caConfiguration){

	state = CA;  // s'esta fent la cronoamperometria

	double eDC= caConfiguration.eDC;

	// eDC es el voltatge constant de la cela electroquimica,
	// el fixem mitjançant la funcio seguent
	sendVoltage(eDC);

	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET); // (set) Tanquem rele

    samplingPeriod=caConfiguration.samplingPeriodMs; // temps entre mostra i mostra (ms)
	uint32_t mTime=caConfiguration.measurementTime; // durada de la crono (s)

	// (durada total de la crono) / (temps entre mostres) = quantes mostres hi ha.
	// COMPTE: Cal passar el Sampling period a segons

	uint8_t measures = (uint8_t)(((double)mTime)/(((double)samplingPeriod)/1000.0)); // nombre de mesures

	// CONFIGURAR EL TIMER

	ClockSettings(samplingPeriod);


	count = 1;  // s'ha fet la primera mesura

	while (count <= measures){  // mentre no arribem al nombre total de mesures
		if (count == measures){  // quan arribem a la mesura final
			state = IDLE;  // no fa ni crono ni cv
		}

		state= CA;
	}

	HAL_TIM_Base_Stop_IT(&htim3);
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET); // reset, OBRIM EL RELÉ
}
