/*
 * stm32main.h
 *
 *  Created on: Apr 26, 2022
 *      Author: eduardruiz
 */

#ifndef INC_COMPONENTS_STM32MAIN_H_
#define INC_COMPONENTS_STM32MAIN_H_

#include "stm32f4xx_hal.h"

struct Handles_S {

		UART_HandleTypeDef *huart2; //UART_HandleTypeDef *huart;
		ADC_HandleTypeDef *hadc1;
		I2C_HandleTypeDef *hi2c1;
		TIM_HandleTypeDef *htim2;
    // Aqui iriamos anadiendo los diferentes XXX_HandleTypeDef que necesitaramos anadir.
};

// Prototypes.

//void setup(struct Handles_S *handles);
void setup();
void loop(void);

#endif /* INC_COMPONENTS_STM32MAIN_H_ */
