/**
 ******************************************************************************
 * @file		cyclic_voltammetry.c
 * @brief		Gestión de la voltammetría cíclica.
 * @author		Albert Álvarez Carulla
 * @copyright	Copyright 2020 Albert Álvarez Carulla. All rights reserved.
 ******************************************************************************
 */

#include "components/cyclic_voltammetry.h"
#include "components/masb_comm_s.h" // en este header se detallan los paquetes de comandos que inician y realizan las mediciones
#include "components/dac.h"
#include "components/adc.h"
#include "components/formulas.h"
#include "main.h"
#include "components/stm32main.h"
// per configurar el voltatge de la cela

#include "components/i2c_lib.h"

extern uint8_t count;
extern volatile uint8_t state;
extern volatile _Bool timeElapsed;
extern TIM_HandleTypeDef htim3; //timer
uint32_t frequency;

const double tol = 1e-6; //tolerancia para determinar si 2 valores son iguales 

_Bool decimalEquals(double num1, double num2, double tolerance) {
	return fabs(num1 - num2) < tolerance;
}

void CyclicVoltammetry(struct CV_Configuration_S cvConfiguration) {

	double ebegin = cvConfiguration.eBegin; // Voltaje inicial que se le asigna a la celda electroquimica

	sendVoltage(ebegin);
	double vObjetivo = cvConfiguration.eVertex1; // voltaje al que debe llegar la funcion (primer vertice)
	double desiredVcell = ebegin;

	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET); //  Cerramos el rele

	uint8_t cycles = cvConfiguration.cycles; // nombre total de ciclos/medidas de la voltimetria ciclica
	double scanRate = cvConfiguration.scanRate; // variación de la tensión de la celda electroquímica en el tiempo
	double eStep = cvConfiguration.eStep; // incremento/decremento de la tensión entre dos puntos consecutivos
	frequency = (uint32_t)(eStep / scanRate * 1000.0); // tiempo entre muestras

	ClockSettings(frequency);

	count = 1; // inicializamos la voltametria.
	state = CV; // mientras se este ejecutando una Voltametria, el estado sera CV
	timeElapsed = FALSE;
	while (cycles) { // mientras no hayamos llegado al ultimo ciclo de la voltametria

		if (timeElapsed) {
			timeElapsed = FALSE;

			// Desde ADC llamamos a la variable Vcell, la cual comparamos com Vobjetivo
			// para ver si ha llegado al vertice 1, vertice 2 o eBegin.

			if (decimalEquals(desiredVcell, vObjetivo, tol)) {

				// ******************* Caso 1: que nos encontremos en el vertice 1  *****************

				if (decimalEquals(vObjetivo, cvConfiguration.eVertex1, tol)) {
					vObjetivo = cvConfiguration.eVertex2; //siguiente objetivo es el vertice 2

				}

				// ***************** Caso 2: que nos encontremos en el vertice 2 *****************
				else if (decimalEquals(vObjetivo, cvConfiguration.eVertex2,
						tol)) {
					vObjetivo = cvConfiguration.eBegin; //objetivo es volver a eBegin

				}

				// ***************** Caso 3: nos encontramos en ebegin *****************
				else {
					vObjetivo = cvConfiguration.eVertex1; //siguiente objetivo es vertice 1
					cycles--;
				}

			}

			else { // Cuando no hemos llegado al objetivo, sumamos o restamos un incremento hasta llegar

				// ***************** Para llegar al vertice 1 *****************

				if (decimalEquals(vObjetivo, cvConfiguration.eVertex1, tol)) {
					if ((desiredVcell + eStep) > vObjetivo) { // sumamos eStep y nos pasamos del objetivo, asi que fijamos el objetivo a vcell
						desiredVcell = vObjetivo;
						sendVoltage(vObjetivo);
					} else { //si sumamos el eStep y no nos pasamos, definimos de nuevo el voltaje de la celda.
						desiredVcell += eStep;
						sendVoltage(desiredVcell);
					}
				}

				// ***************** Para llegar al vertice 2 *****************

				// El vertice dos se encuentra por debajo del vertice 1, así que tendremos que restar el eStep al vcell

				if (decimalEquals(vObjetivo, cvConfiguration.eVertex2, tol)) { //
					if ((desiredVcell - eStep) < vObjetivo) { // Restamos eStep y nos pasamos del objetivo (demasiado pequeño), fijamos el vcell
						desiredVcell = vObjetivo;
						sendVoltage(desiredVcell);
					} else { // si restamos eStep y no nos pasamos, definimos de nuevo el voltaje de la celda.
						desiredVcell -= eStep;
						sendVoltage(desiredVcell);
					}
				}

				// ***************** Para llegar a eBegin *****************

				// El punto inicial se encuentra entre ambos vertices, por debajo del 1 y encima del 2

				if (decimalEquals(vObjetivo, cvConfiguration.eBegin, tol)) {
					if ((desiredVcell + eStep) > vObjetivo) { // sumamos eStep y nos pasamos del objetivo, asi que fijamos el obetivo a vcell
						desiredVcell = vObjetivo;
						sendVoltage(desiredVcell);
					} else { //si sumamos el eStep y no nos pasamos, definimos de nuevo el voltaje de la celda.
						desiredVcell += eStep;
						sendVoltage(desiredVcell);
					}
				}

			}
		}

	}
	HAL_TIM_Base_Stop_IT(&htim3);
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET); //abrimos rele
	state = IDLE;

}

