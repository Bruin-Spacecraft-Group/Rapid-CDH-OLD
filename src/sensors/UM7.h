#ifndef RAPIDCDH_UM7_H
#define RAPIDCDH_UM7_H

#include <cstdint>

#include "../globals.h"

class UM7 {
public:
    enum RegAddr: uint8_t;

    UM7(uint32_t speed);

    [[nodiscard]] Status init();

    // Writes first 4 bytes of data to register
    // Least-significant byte at data[0]
    [[nodiscard]] Status write_reg(RegAddr reg, const uint8_t *data) const;

    // Reads contents of register into first 4 bytes of data
    // Least-significant byte at data[0]
    [[nodiscard]] Status read_reg(RegAddr reg, uint8_t *data) const;

    // Sends command by writing to a command register
    // If getting firmware revision (cmd = 0xAA), 4 bytes are written to data
    [[nodiscard]] Status send_cmd(RegAddr cmd_reg, uint8_t *data = nullptr) const;

    enum RegAddr: uint8_t {
        // Config registers
        CREG_COM_SETTINGS       = 0x00, // General communication settings
        CREG_COM_RATES1         = 0x01, // (1) Broadcast rate settings
        CREG_COM_RATES2         = 0x02, // (2) Broadcast rate settings
        CREG_COM_RATES3         = 0x03, // (3) Broadcast rate settings
        CREG_COM_RATES4         = 0x04, // (4) Broadcast rate settings
        CREG_COM_RATES5         = 0x05, // (5) Broadcast rate settings
        CREG_COM_RATES6         = 0x06, // (6) Broadcast rate settings
        CREG_COM_RATES7         = 0x07, // (7) Broadcast rate settings
        CREG_MISC_SETTINGS      = 0x08,
        CREG_HOME_NORTH         = 0x09, // (9) GPS north position to consider position 0
        CREG_HOME_EAST          = 0x0A, // (10) GPS east position to consider position 0
        CREG_HOME_UP            = 0x0B, // (11)  GPS altitude to consider position 0
        CREG_GYRO_TRIM_X        = 0x0C, // (12) Bias trim for x-axis rate gyro
        CREG_GYRO_TRIM_Y        = 0x0D, // (13) Bias trim for y-axis rate gyro
        CREG_GYRO_TRIM_Z        = 0x0E, // (14) Bias trim for z-axis rate gyro
        CREG_MAG_CAL1_1         = 0x0F, // (15) Row 1, Column 1 of magnetometer calibration matrix
        CREG_MAG_CAL1_2         = 0x10, // (16) Row 1, Column 2 of magnetometer calibration matrix
        CREG_MAG_CAL1_3         = 0x11, // (17) Row 1, Column 3 of magnetometer calibration matrix
        CREG_MAG_CAL2_1         = 0x12, // (18) Row 2, Column 1 of magnetometer calibration matrix
        CREG_MAG_CAL2_2         = 0x13, // (19) Row 2, Column 2 of magnetometer calibration matrix
        CREG_MAG_CAL2_3         = 0x14, // (20) Row 2, Column 3 of magnetometer calibration matrix
        CREG_MAG_CAL3_1         = 0x15, // (21) Row 3, Column 1 of magnetometer calibration matrix
        CREG_MAG_CAL3_2         = 0x16, // (22) Row 3, Column 2 of magnetometer calibration matrix
        CREG_MAG_CAL3_3         = 0x17, // (23) Row 3, Column 3 of magnetometer calibration matrix
        CREG_MAG_BIAS_X         = 0x18, // (24) Magnetometer X-axis bias
        CREG_MAG_BIAS_Y         = 0x19, // (25) Magnetometer Y-axis bias
        CREG_MAG_BIAS_Z         = 0x1A, // (26) Magnetometer Z-axis bias
        CREG_ACCEL_CAL1_1       = 0x1B, // Row 1, Column 1 of accelerometer calibration matrix
        CREG_ACCEL_CAL1_2       = 0x1C, // Row 1, Column 2 of accelerometer calibration matrix
        CREG_ACCEL_CAL1_3       = 0x1D, // Row 1, Column 3 of accelerometer calibration matrix
        CREG_ACCEL_CAL2_1       = 0x1E, // Row 2, Column 1 of accelerometer calibration matrix
        CREG_ACCEL_CAL2_2       = 0x1F, // Row 2, Column 2 of accelerometer calibration matrix
        CREG_ACCEL_CAL2_3       = 0x20, // Row 2, Column 3 of accelerometer calibration matrix
        CREG_ACCEL_CAL3_1       = 0x21, // Row 3, Column 1 of accelerometer calibration matrix
        CREG_ACCEL_CAL3_2       = 0x22, // Row 3, Column 2 of accelerometer calibration matrix
        CREG_ACCEL_CAL3_3       = 0x23, // Row 3, Column 3 of accelerometer calibration matrix
        CREG_ACCEL_BIAS_X       = 0x24, // Accelerometer X-axis bias
        CREG_ACCEL_BIAS_Y       = 0x25, // Accelerometer Y-axis bias
        CREG_ACCEL_BIAS_Z       = 0x26, // Accelerometer Z-axis bias

        // Data Registers
        DREG_HEALTH              = 0x55, // (85)  Contains information about the health and status of the UM7
        DREG_GYRO_RAW_XY         = 0x56, // (86)  Raw X and Y rate gyro data
        DREG_GYRO_RAW_Z          = 0x57, // (87) Raw Z rate gyro data
        DREG_GYRO_TIME           = 0x58, // (88) Time at which rate gyro data was acquired
        DREG_ACCEL_RAW_XY        = 0x59, // (89) Raw X and Y accelerometer data
        DREG_ACCEL_RAW_Z         = 0x5A, // (90) Raw Z accelerometer data
        DREG_ACCEL_TIME          = 0x5B, // (91) Time at which accelerometer data was acquired
        DREG_MAG_RAW_XY          = 0x5C, // (92) Raw X and Y magnetometer data
        DREG_MAG_RAW_Z           = 0x5D, // (93) Raw Z magnetometer data
        DREG_MAG_RAW_TIME        = 0x5E, // (94) Time at which magnetometer data was acquired
        DREG_TEMPERATURE         = 0x5F, // (95) Temperature data
        DREG_TEMPERATURE_TIME    = 0x60, // (96) Time at which temperature data was acquired
        DREG_GYRO_PROC_X         = 0x61, // (97) Processed x-axis rate gyro data
        DREG_GYRO_PROC_Y         = 0x62, // (98) Processed y-axis rate gyro data
        DREG_GYRO_PROC_Z         = 0x63, // (99)  Processed z-axis rate gyro data
        DREG_GYRO_PROC_TIME      = 0x64, // (100) Time at which rate gyro data was acquired
        DREG_ACCEL_PROC_X        = 0x65, // (101) Processed x-axis accel data
        DREG_ACCEL_PROC_Y        = 0x66, // (102) Processed y-axis accel data
        DREG_ACCEL_PROC_Z        = 0x67, // (103) Processed z-axis accel data
        DREG_ACCEL_PROC_TIME     = 0x68, // (104) Time at which accelerometer data was acquired
        DREG_MAG_PROC_X          = 0x69, // (105) Processed x-axis magnetometer data
        DREG_MAG_PROC_Y          = 0x6A, // (106) Processed y-axis magnetometer data
        DREG_MAG_PROC_Z          = 0x6B, // (107) Processed z-axis magnetometer data
        DREG_MAG_PROC_TIME       = 0x6C, // (108) Time at which magnetometer data was acquired
        DREG_QUAT_AB             = 0x6D, //(109) Quaternion elements A and B
        DREG_QUAT_CD             = 0x6E, //(110)  Quaternion elements C and D
        DREG_QUAT_TIME           = 0x6F, // (111) Time at which the sensor was at the specified quaternion rotation
        DREG_EULER_PHI_THETA     = 0x70, // (112)  Roll and pitch angles
        DREG_EULER_PSI           = 0x71, // Yaw angle
        DREG_EULER_PHI_THETA_DOT = 0x72, // Roll and pitch angle rates
        DREG_EULER_PSI_DOT       = 0x73, // Yaw rate
        DREG_EULER_TIME          = 0x74, // Time of computed Euler attitude and rates
        DREG_POSITION_NORTH      = 0x75, // North position in meters
        DREG_POSITION_EAST       = 0x76, // East position in meters
        DREG_POSITION_UP         = 0x77, // Altitude in meters
        DREG_POSITION_TIME       = 0x78, // Time of estimated position
        DREG_VELOCITY_NORTH      = 0x79, // North velocity
        DREG_VELOCITY_EAST       = 0x7A, // East velocity
        DREG_VELOCITY_UP         = 0x7B, // Altitude velocity
        DREG_VELOCITY_TIME       = 0x7C, // Time of velocity estimate
        DREG_GPS_LATITUDE        = 0x7D, // GPS latitude
        DREG_GPS_LONGITUDE       = 0x7E, // GPS longitude
        DREG_GPS_ALTITUDE        = 0x7F, // GPS altitude
        DREG_GPS_COURSE          = 0x80, // GPS course
        DREG_GPS_SPEED           = 0x81, // GPS speed
        DREG_GPS_TIME            = 0x82, // GPS time (UTC time of day in seconds)
        DREG_GPS_SAT_1_2         = 0x83, // GPS satellite information
        DREG_GPS_SAT_3_4         = 0x84, // GPS satellite information
        DREG_GPS_SAT_5_6         = 0x85, // GPS satellite information
        DREG_GPS_SAT_7_8         = 0x86, // GPS satellite information
        DREG_GPS_SAT_9_10        = 0x87, // GPS satellite information
        DREG_GPS_SAT_11_12       = 0x88, // GPS satellite information
        DREG_GYRO_BIAS_X         = 0x89, // Gyro x-axis bias estimate
        DREG_GYRO_BIAS_Y         = 0x8A, // Gyro y-axis bias estimate
        DREG_GYRO_BIAS_Z         = 0x8B, // Gyro z-axis bias estimate

        // Command registers
        GET_FW_REVISION          = 0xAA, // Causes the autopilot to respond with a packet containing the current firmware revision.
        FLASH_COMMIT             = 0xAB, // Writes all current configuration settings to flash
        RESET_TO_FACTORY         = 0xAC, // Reset all settings to factory defaults
        ZERO_GYROS               = 0xAD, // Causes the rate gyro biases to be calibrated.
        SET_HOME_POSITION        = 0xAE, // Sets the current GPS location as position (0,0)
        SET_MAG_REFERENCE        = 0xB0, // Sets the magnetometer reference vector
        CALIBRATE_ACCELEROMETERS = 0xB1, // Calibrates the accelerometer biases
        RESET_EKF                = 0xB3  // Resets the EKF
    };

private:
    uint8_t m_channel;
    uint32_t m_speed;
    int32_t m_spi;

    static constexpr uint8_t CHANNEL = 1; // Chip select (0 or 1)
    static constexpr uint8_t OP_LEN  = 6; // Number of bytes per SPI operation
};

#endif //RAPIDCDH_UM7_H