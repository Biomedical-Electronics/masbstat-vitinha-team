/*
 * cyclic_voltammetry.h
 *
 *  Created on: 3 may. 2022
 *      Author: eduardruiz
 */


#ifndef INC_COMPONENTS_CYCLIC_VOLTAMMETRY_H_
#define INC_COMPONENTS_CYCLIC_VOLTAMMETRY_H_

#include "stm32f4xx_hal.h"

struct CV_Configuration_S {

	double eBegin;
	double eVertex1;
	double eVertex2;
	uint8_t cycles;
	double scanRate;
	double eStep;

};

void Cyclic_Voltammetry(struct CV_Configuration_S cvConfiguration);


#endif /* INC_COMPONENTS_CYCLIC_VOLTAMMETRY_H_ */
