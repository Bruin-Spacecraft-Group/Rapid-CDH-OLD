#include <wiringPiSPI.h>

#include "UM7.h"

UM7::UM7(uint8_t channel, uint32_t speed)
    : m_channel(channel), m_speed(speed) {}

Status UM7::init() {
    if ((m_spi = wiringPiSPISetup(m_channel, (int) m_speed)) < 0) {
        return FAILURE;
    }

    return SUCCESS;
}

