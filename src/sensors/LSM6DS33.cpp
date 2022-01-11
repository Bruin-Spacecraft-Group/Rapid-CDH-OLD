#include <iostream>

#include <wiringPiI2C.h>

#include "LSM6DS33.h"

using std::cout;
using std::cerr;
using std::endl;

LSM6DS33::LSM6DS33(const char *i2c_dev, uint8_t dev_id)
    : m_i2c_dev(i2c_dev), m_dev_id(dev_id) {}

Status LSM6DS33::init() {
    if ((m_i2c_fd = wiringPiI2CSetupInterface(m_i2c_dev, m_dev_id)) < 0) {
        return FAILURE;
    }

    return SUCCESS;
}

Status LSM6DS33::write_reg(LSM6DS33::RegAddr reg, uint8_t data) const {
    if (wiringPiI2CWriteReg8((int) m_i2c_fd, reg, data) == -1) {
        cerr << "Failed to write to register: " << (int) reg << endl;
        return FAILURE;
    }

    return SUCCESS;
}

Status LSM6DS33::set_accel_settings(LSM6DS33::ODR odr, LSM6DS33::AccelFS fs, LSM6DS33::AccelBW bw) {
    uint8_t data = 0b11110000 & odr;
    data |= 0b00001100 & fs;
    data |= 0b00000011 & bw;

    Status status = write_reg(CTRL1_XL, data);
    if (!status) return FAILURE;

    m_accel_odr = enum_to_odr(odr);

    switch (fs) {
        case A_FS_2G:
            m_accel_fs = 2;
            break;
        case A_FS_4G:
            m_accel_fs = 4;
            break;
        case A_FS_8G:
            m_accel_fs = 8;
            break;
        case A_FS_16G:
            m_accel_fs = 16;
            break;
    }

    switch (bw) {
        case A_BW_400HZ:
            m_accel_bw = 400;
            break;
        case A_BW_200HZ:
            m_accel_bw = 200;
            break;
        case A_BW_100HZ:
            m_accel_bw = 100;
            break;
        case A_BW_50HZ:
            m_accel_bw = 50;
            break;
    }

    return SUCCESS;
}

Status LSM6DS33::set_gyro_settings(LSM6DS33::ODR odr, LSM6DS33::GyroFS fs) {
    uint8_t data = 0b11110000 & odr;
    data |= 0b00001110 & fs;

    Status status = write_reg(CTRL2_G, data);
    if (!status) return FAILURE;

    m_gyro_odr = enum_to_odr(odr);

    switch (fs) {
        case G_FS_125DPS:
            m_gyro_fs = 125;
            break;
        case G_FS_250DPS:
            m_gyro_fs = 250;
            break;
        case G_FS_500DPS:
            m_gyro_fs = 500;
            break;
        case G_FS_1000DPS:
            m_gyro_fs = 1000;
            break;
        case G_FS_2000DPS:
            m_gyro_fs = 2000;
            break;
    }

    return SUCCESS;
}

uint32_t LSM6DS33::enum_to_odr(LSM6DS33::ODR odr) {
    switch (odr) {
        case ODR_12HZ:
            return 12;
        case ODR_26HZ:
            return 26;
        case ODR_52HZ:
            return 52;
        case ODR_104HZ:
            return 104;
        case ODR_208HZ:
            return 208;
        case ODR_416HZ:
            return 416;
        case ODR_833HZ:
            return 833;
        case ODR_1660HZ:
            return 1660;
        case ODR_3330HZ:
            return 3330;
        case ODR_6660HZ:
            return 6660;
    }
}