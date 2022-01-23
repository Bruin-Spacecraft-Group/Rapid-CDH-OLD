#include <iostream>

#include <wiringPiSPI.h>

#include "UM7.h"

using std::cout;
using std::cerr;
using std::endl;

UM7::UM7(uint32_t speed)
    : m_speed(speed) {}

Status UM7::init() {
    if ((m_spi = wiringPiSPISetup(CHANNEL, (int) m_speed)) < 0) {
        return FAILURE;
    }

    return SUCCESS;
}

Status UM7::write_reg(RegAddr reg, const uint8_t *data) const {
    uint8_t op[OP_LEN] = {0x01, reg, data[3], data[2], data[1], data[0]};
    int32_t res = wiringPiSPIDataRW(CHANNEL, op, OP_LEN);
    if (res < 0) {
        cerr << "Failed to write to register " << (int) reg << endl;
        return FAILURE;
    }

    return SUCCESS;
}

Status UM7::read_reg(RegAddr reg, uint8_t *data) const {
    uint8_t op[OP_LEN] = {0x00, reg, 0x00, 0x00, 0x00, 0x00};
    int32_t res = wiringPiSPIDataRW(CHANNEL, op, OP_LEN);
    if (res < 0) {
        cerr << "Failed to read from register " << (int) reg << endl;
        return FAILURE;
    }

    // Copy response to data
    // Most-significant byte written first
    data[0] = op[3];
    data[1] = op[2];
    data[2] = op[1];
    data[3] = op[0];

    return SUCCESS;
}

Status UM7::send_cmd(RegAddr cmd_reg, uint8_t *data) const {
    uint8_t op[OP_LEN] = {0x01, cmd_reg, 0x00, 0x00, 0x00, 0x00};
    int32_t res = wiringPiSPIDataRW(CHANNEL, op, OP_LEN);
    if (res < 0) {
        cerr << "Failed to write to register " << (int) cmd_reg << endl;
        return FAILURE;
    }

    if (cmd_reg == GET_FW_REVISION && data != nullptr) {
        // Copy firmware revision data
        data[0] = op[3];
        data[1] = op[2];
        data[2] = op[1];
        data[3] = op[0];
    }

    return SUCCESS;
}
