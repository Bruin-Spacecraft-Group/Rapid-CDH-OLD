#ifndef RAPIDCDH_LSM6DS33_H
#define RAPIDCDH_LSM6DS33_H

#include <cstdint>

#include "../globals.h"

class LSM6DS33 {
public:
    // Enum forward declarations
    enum RegAddr : uint8_t;
    enum ODR     : uint8_t;
    enum AccelFS : uint8_t;
    enum AccelBW : uint8_t;
    enum GyroFS  : uint8_t;
    enum FifoODR : uint8_t;
    enum FifoMode: uint8_t;

    explicit LSM6DS33(const char* i2c_dev, uint8_t dev_id = constants::LSM6DS33_DEV_ID);

    [[nodiscard]] Status init();
    [[nodiscard]] Status set_accel(ODR odr, AccelFS fs = A_FS_2G, AccelBW bw = A_BW_400HZ);
    [[nodiscard]] Status set_gyro(ODR odr, GyroFS fs = G_FS_250DPS);

    // FIFO config
    [[nodiscard]] Status fifo_init(FifoMode mode, FifoODR odr); // FIFO setup
    [[nodiscard]] Status set_fifo(FifoMode mode, FifoODR odr); // Set FIFO mode and ODR
    [[nodiscard]] Status reset_fifo() const; // Resets FIFO when in FIFO mode

    [[nodiscard]] Status write_reg(RegAddr reg, uint8_t data) const;
    [[nodiscard]] Status read_reg_8(RegAddr reg, uint8_t& data) const;
    [[nodiscard]] Status read_reg_16(RegAddr reg, uint16_t& data) const;

    [[nodiscard]] Status read_new_data(); // Checks for new data and reads from OUT registers
    [[nodiscard]] Status read_fifo(); // Read from FIFO registers

    void print_data() const;
    void print_settings() const;

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

    // Accel full-scale selection (g)
    // Set FS_XL[1:0] in CTRL1_XL
    enum AccelFS: uint8_t {
        A_FS_2G, // Default
        A_FS_4G,
        A_FS_8G,
        A_FS_16G
    };

    // Accelerometer anti-aliasing filter bandwidth selection
    // Set BW_XL[1:0] in CTRL1_XL
    enum AccelBW: uint8_t {
        A_BW_400HZ, // Default
        A_BW_200HZ,
        A_BW_100HZ,
        A_BW_50HZ
    };

    // Gyro full-scale selection (dps)
    // Set FS_G[1:0], FS_125 in CTRL2_G
    enum GyroFS: uint8_t {
        G_FS_125DPS  = 0b001,
        G_FS_250DPS  = 0b000, // Default
        G_FS_500DPS  = 0b010,
        G_FS_1000DPS = 0b100,
        G_FS_2000DPS = 0b110
    };

    // FIFO ODR
    // Set ODR_FIFO[3:0] in FIFO_CTRL5
    enum FifoODR: uint8_t {
        FIFO_DISABLED, // Default
        FIFO_ODR_12_5HZ,
        FIFO_ODR_26HZ,
        FIFO_ODR_52HZ,
        FIFO_ODR_104HZ,
        FIFO_ODR_208HZ,
        FIFO_ODR_416HZ,
        FIFO_ODR_833HZ,
        FIFO_ODR_1660HZ,
        FIFO_ODR_3330HZ,
        FIFO_ODR_6660HZ
    };

    // FIFO mode selection
    // Set FIFO_MODE[2:0] in FIFO_CTRL5
    enum FifoMode: uint8_t {
        FIFO_MODE_BYPASS         = 0b000, // Default
        FIFO_MODE_FIFO           = 0b001,
        FIFO_MODE_CONT_TO_FIFO   = 0b011,
        FIFO_MODE_BYPASS_TO_CONT = 0b100,
        FIFO_MODE_CONT           = 0b110
    };

    // Register address mapping
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

    // Control registers
    uint8_t m_accel_ctrl = 0; // CTRL1_XL
    uint8_t m_gyro_ctrl  = 0; // CTRL2_G
    uint8_t m_fifo_ctrl  = 0; // FIFO_CTRL5

    // Raw output data
    uint16_t m_out_temp;
    uint16_t m_out_gyro_x, m_out_gyro_y, m_out_gyro_z;
    uint16_t m_out_accel_x, m_out_accel_y, m_out_accel_z;

    // Temperature sensitivity: LSB/degree C
    static constexpr float TEMP_SENS = 16.0;

    // Accelerometer sensitivity: mg/LSB
    static constexpr float A_SENS_2G  = 0.061;
    static constexpr float A_SENS_4G  = 0.122;
    static constexpr float A_SENS_8G  = 0.244;
    static constexpr float A_SENS_16G = 0.488;

    // Gyroscope sensitivity: mdps/LSB
    static constexpr float G_SENS_125DPS  = 4.375;
    static constexpr float G_SENS_250DPS  = 8.75;
    static constexpr float G_SENS_500DPS  = 17.50;
    static constexpr float G_SENS_1000DPS = 35;
    static constexpr float G_SENS_2000DPS = 70;

    static uint32_t enum_to_odr(ODR odr);
};


#endif //RAPIDCDH_LSM6DS33_H
