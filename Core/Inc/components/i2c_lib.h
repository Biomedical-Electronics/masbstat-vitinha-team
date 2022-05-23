#ifndef __I2C_LIB_H
#define __I2C_LIB_H

#include <stdint.h>
#include "stm32f4xx_hal.h"

// Comentar esta linea cuando se tenga la placa disponible.
#define I2C_DEBUG

void I2C_init(I2C_HandleTypeDef *i2cHandler);
void I2C_write(uint8_t slaveAddress, uint8_t *data, uint16_t length);

#endif
