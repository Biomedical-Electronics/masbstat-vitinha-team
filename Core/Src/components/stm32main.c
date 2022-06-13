/*
 * stm32main.c
 *
 *  Created on: Apr 26, 2022
 *      Author: eduardruiz
 */


#include "components/stm32main.h"
#include "components/masb_comm_s.h"
#include "components/i2c_lib.h"
#include "components/mcp4725_driver.h"
#include "components/ad5280_driver.h"
#include "main.h"
// The structured variables for the CA and CV are initialized. Also a structure to send data
struct CV_Configuration_S cvConfiguration;
struct CA_Configuration_S caConfiguration;
struct Data_S data;
// Some peripherals are imported (UART and I2C) and two are initialized (dac and potentiometer)
AD5280_Handle_T hpot = NULL;
MCP4725_Handle_T hdac = NULL;
extern UART_HandleTypeDef huart2;
extern I2C_HandleTypeDef hi2c1;


// Function of setup: I2C, DAC and potentiometer are initialized. The plate is alimented.
void setup() {

	        HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, GPIO_PIN_SET);
	        HAL_Delay(500);
	        I2C_init(&hi2c1);
	        hpot = AD5280_Init();
	    	AD5280_ConfigSlaveAddress(hpot, 0x2C);
	    	AD5280_ConfigNominalResistorValue(hpot, 50e3f);
	    	AD5280_ConfigWriteFunction(hpot, I2C_write);
	    	AD5280_SetWBResistance(hpot, 50e3f);

	    	hdac = MCP4725_Init();
	    	MCP4725_ConfigSlaveAddress(hdac, 0x66);
	    	MCP4725_ConfigVoltageReference(hdac, 4.0f);
	    	MCP4725_ConfigWriteFunction(hdac, I2C_write);

	    	MASB_COMM_S_waitForMessage();
}

// main loop function
void loop(void) {

   if (MASB_COMM_S_dataReceived()) { // If a message is received

		switch(MASB_COMM_S_command()) { //The command received is evaluated

 			case START_CV_MEAS: // If case is START_CV_MEAS

 				// THE CV configuration received via UART COM is saved in teh cvConfiguration
 				//structure and the cyclic voltammetry function is executed
				cvConfiguration = MASB_COMM_S_getCvConfiguration();
				Cyclic_Voltammetry(cvConfiguration);

 				break;

 			case START_CA_MEAS: // If  START_CA_MEAS is received

				 // The configuration of the CA received via UART is saved in CA data
 				//structure and the CA function is executed
				caConfiguration = MASB_COMM_S_getCaConfiguration();
				ChronoAmperometry(caConfiguration);

				break;

			case STOP_MEAS: // If STOP_MEAS is received


 				__NOP();

 				break;


 		}

       // After processing the command, another command is awaited
 		MASB_COMM_S_waitForMessage();

 	}

 	// Aqui es donde deberia de ir el codigo de control de las mediciones si se quiere implementar
   // el comando de STOP.

}
