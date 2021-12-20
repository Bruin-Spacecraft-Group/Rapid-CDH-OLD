#ifndef GLOBALS_H
#define GLOBALS_H

// WiringPi pins
const int UCAM_RESET_PIN = 1;

// Serial
const char* SERIAL_DEV_0 = "/dev/ttyAMA0";
const char* SERIAL_DEV_1 = "/dev/ttyS0";
const int SERIAL_BAUD_RATE = 115200;

// arbritary mapping for function implementations in sensors.h and more
enum sensor_type {
    ACCEL,
    GYRO,
    TEMP,
    BARO,
    CURRENT,
    MAG
};

enum flight_state {
    // status constants (as currently used in systems.h)
    DISCONNECTED,
    // maybe this as a general status for being mission ready before launch? (this would be right after turn-on)
    IDLE,
    READY_TO_RECIEVE,
    READY_TO_SEND,
};

// return codes (success and error types)
enum result_type {
    RESULT_SUCCESS,
    RESULT_FALSE,
    ERROR_ADDR,
    ERROR_POWER,
    ERROR_DATAFORMAT,
    ERROR_READ,
    ERROR_POLL,
    ERROR_INVALID_STATUS
};

// sensor status types
enum sensor_status_type {
    STATUS_IDLE,
    STATUS_OFF,
    STATUS_ERROR
};

#endif
