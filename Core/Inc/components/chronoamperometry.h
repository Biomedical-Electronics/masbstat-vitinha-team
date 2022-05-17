/*
 * chronoamperometry.h
 *
 *  Created on: Apr 26, 2022
 *      Author: eduardruiz
 */


#ifndef INC_COMPONENTS_CHRONOAMPEROMETRY_H_
#define INC_COMPONENTS_CHRONOAMPEROMETRY_H_

#include "stm32f4xx_hal.h"
#include "math.h"

struct CA_Configuration_S {

	double eDC;
	uint32_t samplingPeriodMs;
	uint32_t measurementTime;

};

#endif /* INC_COMPONENTS_CHRONOAMPEROMETRY_H_ */

void ChronoAmperometry(struct CA_Configuration_S caConfiguration);



