#include "components/i2c_lib.h"

static I2C_HandleTypeDef *hi2c;

void I2C_init(I2C_HandleTypeDef *i2cHandler) {

    hi2c = i2cHandler;
    
}

void I2C_write(uint8_t slaveAddress, uint8_t *data, uint16_t length) {

#ifndef I2C_DEBUG
     HAL_I2C_Master_Transmit(hi2c, slaveAddress << 1, data, length, 100);
#endif

} 