#ifndef RAPIDCDH_LSM6DS33_H
#define RAPIDCDH_LSM6DS33_H

#include <cstdint>

#include "../globals.h"

class LSM6DS33 {
public:
    // Enum forward declarations
    enum RegAddr: uint8_t;
    enum ODR: uint8_t;

    LSM6DS33(const char* i2c_dev, uint8_t dev_id);

    [[nodiscard]] Status init();

    // Enums

    // Output Data Rate
    // Set ODR_XL[3:0] in CTRL1_XL for accel
    // Set ODR_G[3:0] in CTRL2_G for gyro
    enum ODR: uint8_t {
        ODR_12HZ   = 0b0001,
        ODR_26HZ   = 0b0010,
        ODR_52HZ   = 0b0011,
        ODR_104HZ  = 0b0100,
        ODR_208HZ  = 0b0101,
        ODR_416HZ  = 0b0110,
        ODR_833HZ  = 0b0111,
        ODR_1660HZ = 0b1000,
        ODR_3330HZ = 0b1001, // Accel only
        ODR_6660HZ = 0b1010  // Accel only
    };

    enum RegAddr: uint8_t {
        // Embedded functions config
        FUNC_CFG_ACCESS   = 0x01,

        // FIFO config
        FIFO_CTRL1        = 0x06,
        FIFO_CTRL2        = 0x07,
        FIFO_CTRL3        = 0x08,
        FIFO_CTRL4        = 0x09,
        FIFO_CTRL5        = 0x0A,

        ORIENT_CFG_G      = 0x0B,

        // INT 1/2 pin control
        INT1_CTRL         = 0x0D,
        INT2_CTRL         = 0x0E,

        // Who I am ID
        WHO_AM_I          = 0x0F,

        // Accelerometer and gyroscope control
        CTRL1_XL          = 0x10,
        CTRL2_G           = 0x11,
        CTRL3_C           = 0x12,
        CTRL4_C           = 0x13,
        CTRL5_C           = 0x14,
        CTRL6_C           = 0x15,
        CTRL7_G           = 0x16,
        CTRL8_XL          = 0x17,
        CTRL9_XL          = 0x18,
        CTRL10_C          = 0x19,

        // Interrupts
        WAKE_UP_SRC       = 0x1B,
        TAP_SRC           = 0x1C,
        D6D_SRC           = 0x1D,

        // Status data
        STATUS_REG        = 0x1E,

        // Temperature output data
        OUT_TEMP_L        = 0x20,
        OUT_TEMP_H        = 0x21,

        // Gyroscope output
        OUTX_L_G          = 0x22,
        OUTX_H_G          = 0x23,
        OUTY_L_G          = 0x24,
        OUTY_H_G          = 0x25,
        OUTZ_L_G          = 0x26,
        OUTZ_H_G          = 0x27,

        // Accelerometer output
        OUTX_L_XL         = 0x28,
        OUTX_H_XL         = 0x29,
        OUTY_L_XL         = 0x2A,
        OUTY_H_XL         = 0x2B,
        OUTZ_L_XL         = 0x2C,
        OUTZ_H_XL         = 0x2D,

        // FIFO status
        FIFO_STATUS1      = 0x3A,
        FIFO_STATUS2      = 0x3B,
        FIFO_STATUS3      = 0x3C,
        FIFO_STATUS4      = 0x3D,

        // FIFO data output
        FIFO_DATA_OUT_L   = 0x3E,
        FIFO_DATA_OUT_H   = 0x3F,

        // Timestamp output
        TIMESTAMP0_REG    = 0x40,
        TIMESTAMP1_REG    = 0x41,
        TIMESTAMP2_REG    = 0x42,

        // Step counter timestamp
        STEP_TIMESTAMP_L  = 0x49,
        STEP_TIMESTAMP_H  = 0x4A,

        // Step counter output
        STEP_COUNTER_L    = 0x4B,
        STEP_COUNTER_H    = 0x4C,

        // Interrupts
        FUNC_SRC          = 0x53,
        TAP_CFG           = 0x58,
        TAP_THS_6D        = 0x59,
        INT_DUR2          = 0x5A,
        WAKE_UP_THS       = 0x5B,
        WAKE_UP_DUR       = 0x5C,
        FREE_FALL         = 0x5D,
        MD1_CFG           = 0x5E,
        MD2_CFG           = 0x5F,
    };

private:
    const char* m_i2c_dev;
    uint8_t m_dev_id;
    uint32_t m_i2c_fd;
};

#endif //RAPIDCDH_LSM6DS33_H
