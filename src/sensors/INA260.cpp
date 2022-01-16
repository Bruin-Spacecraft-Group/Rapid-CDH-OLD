#include <iostream>
#include <wiringPiI2C.h>

#include "INA260.h"

using std::cout;
using std::cerr;
using std::endl;

INA260::INA260(const char *device, uint8_t dev_id)
    : m_i2c_dev(device), m_dev_id(dev_id) {}

Status INA260::init() {
    if ((m_i2c = wiringPiI2CSetupInterface(m_i2c_dev, m_dev_id)) < 0) {
        return FAILURE;
    }

    return SUCCESS;
}

Status INA260::write_reg_16(RegAddr reg, uint16_t data) const {
    int32_t result;
    if ((result = wiringPiI2CWriteReg16(m_i2c, reg, data)) == -1) {
        cerr << "Failed to write to register: " << (int) reg << endl;
        return FAILURE;
    }

    return SUCCESS;
}

Status INA260::read_reg_16(INA260::RegAddr reg, uint16_t &data) const {
    int32_t result;
    if ((result = wiringPiI2CReadReg16(m_i2c, reg)) == -1) {
        cerr << "Failed read register: " << (int) reg << endl;
        return FAILURE;
    }

    data = (uint16_t) result;

    return SUCCESS;
}

Status INA260::reset() const {
    Status status = write_reg_16(REG_CONFIG, 0x8000);
    if (!status) return status;

    return SUCCESS;
}

Status INA260::read_current() {
    return read_reg_16(REG_CURRENT, m_current);
}

void INA260::get_current(float &current) const {
    current = (float) m_current * CURRENT_SENS;
}

