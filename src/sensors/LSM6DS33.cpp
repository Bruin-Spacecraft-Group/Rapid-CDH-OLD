#include <wiringPiI2C.h>

#include "LSM6DS33.h"

LSM6DS33::LSM6DS33(const char *i2c_dev, uint8_t dev_id)
    : m_i2c_dev(i2c_dev), m_dev_id(dev_id) {}

Status LSM6DS33::init() {
    if ((m_i2c_fd = wiringPiI2CSetupInterface(m_i2c_dev, m_dev_id)) < 0) {
        return FAILURE;
    }

    return SUCCESS;
}
