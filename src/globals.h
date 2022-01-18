#ifndef RAPIDCDH_GLOBALS_H
#define RAPIDCDH_GLOBALS_H

#include <cstdint>

namespace constants {
    inline constexpr bool DEBUG = true;

    // WiringPi pins
    inline constexpr uint8_t UCAM_RESET_PIN = 7;

    // Serial
    inline const char* SERIAL_DEV_0 = "/dev/ttyAMA0";
    inline constexpr uint32_t SERIAL_BAUD_RATE = 921600;
}

// Error codes
enum [[nodiscard]] Status {
    SUCCESS,
    FAILURE,
    I2C_SETUP_FAILURE,
    I2C_WRITE_FAILURE,
    I2C_READ_FAILURE,
    INVALID_INPUT,
};

#endif
