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

Status LSM6DS33::read_reg_8(LSM6DS33::RegAddr reg, uint8_t& data) const {
    int32_t result;
    if ((result = wiringPiI2CReadReg8((int) m_i2c_fd, reg)) == -1) {
        cerr << "Failed to read register: " << (int) reg << endl;
        return FAILURE;
    }

    data = (uint8_t) result;

    return SUCCESS;
}

Status LSM6DS33::read_reg_16(LSM6DS33::RegAddr reg, uint16_t& data) const {
    int32_t result;
    if ((result = wiringPiI2CReadReg16((int) m_i2c_fd, reg)) == -1) {
        cerr << "Failed to read register: " << (int) reg << endl;
        return FAILURE;
    }

    data = (uint16_t) result;

    return SUCCESS;
}

Status LSM6DS33::set_accel_settings(LSM6DS33::ODR odr, LSM6DS33::AccelFS fs, LSM6DS33::AccelBW bw) {
    uint8_t data = 0b11110000 & odr;
    data |= 0b00001100 & fs;
    data |= 0b00000011 & bw;

    Status status = write_reg(CTRL1_XL, data);
    if (!status) return FAILURE;

    m_accel_ctrl = data;

    return SUCCESS;
}

Status LSM6DS33::set_gyro_settings(LSM6DS33::ODR odr, LSM6DS33::GyroFS fs) {
    uint8_t data = 0b11110000 & odr;
    data |= 0b00001110 & fs;

    Status status = write_reg(CTRL2_G, data);
    if (!status) return FAILURE;

    m_gyro_ctrl = data;

    return SUCCESS;
}

Status LSM6DS33::read_new_data() {
    uint8_t status_reg;
    uint16_t data;

    // Read status register to check for new data
    Status status = read_reg_8(STATUS_REG, status_reg);
    if (!status) return status;

    // Check for new temperature data (STATUS_REG[2] == 1)
    if ((status_reg & 0b00000100) >> 2) {
        status = read_reg_16(OUT_TEMP_L, data);
        if (!status) return status;

        m_out_temp = data;
    }

    // Check for new gyroscope data (STATUS_REG[1] == 1)
    if ((status_reg & 0b00000010) >> 1) {
        status = read_reg_16(OUTX_L_G, data);
        if (!status) return status;
        m_out_gyro_x = data;

        status = read_reg_16(OUTY_L_G, data);
        if (!status) return status;
        m_out_gyro_y = data;

        status = read_reg_16(OUTZ_L_G, data);
        if (!status) return status;
        m_out_gyro_z = data;
    }

    // Check for new accelerometer data (STATUS_REG[0] == 1)
    if (status_reg & 0b00000001) {
        status = read_reg_16(OUTX_L_XL, data);
        if (!status) return status;
        m_out_accel_x = data;

        status = read_reg_16(OUTY_L_XL, data);
        if (!status) return status;
        m_out_accel_y = data;

        status = read_reg_16(OUTZ_L_XL, data);
        if (!status) return status;
        m_out_accel_z = data;
    }

    return SUCCESS;
}

void LSM6DS33::print_data() const {
    float accel_sens, gyro_sens;

    switch ((m_accel_ctrl & 0b00001100) >> 2) {
        case A_FS_2G:
            accel_sens = A_SENS_2G;
            break;
        case A_FS_4G:
            accel_sens = A_SENS_4G;
            break;
        case A_FS_8G:
            accel_sens = A_SENS_8G;
            break;
        case A_FS_16G:
            accel_sens = A_SENS_16G;
            break;
    }

    switch ((m_gyro_ctrl & 0b00001110) >> 1) {
        case G_FS_125DPS:
            gyro_sens = G_SENS_125DPS;
            break;
        case G_FS_250DPS:
            gyro_sens = G_SENS_250DPS;
            break;
        case G_FS_500DPS:
            gyro_sens = G_SENS_500DPS;
            break;
        case G_FS_1000DPS:
            gyro_sens = G_SENS_1000DPS;
            break;
        case G_FS_2000DPS:
            gyro_sens = G_SENS_2000DPS;
            break;
    }

    cout << "Temperature: " << (float) m_out_temp / TEMP_SENS     << endl;
    cout << "Accel X: "     << (float) m_out_accel_x * accel_sens << endl;
    cout << "Accel Y: "     << (float) m_out_accel_y * accel_sens << endl;
    cout << "Accel Z: "     << (float) m_out_accel_z * accel_sens << endl;
    cout << "Gyro X: "      << (float) m_out_gyro_x * gyro_sens   << endl;
    cout << "Gyro Y: "      << (float) m_out_gyro_y * gyro_sens   << endl;
    cout << "Gyro Z: "      << (float) m_out_gyro_z * gyro_sens   << endl;
}

void LSM6DS33::print_settings() const {
    uint32_t accel_odr, gyro_odr;
    uint16_t accel_fs, accel_bw, gyro_fs;

    accel_odr = enum_to_odr((ODR) (m_accel_ctrl >> 4));
    gyro_odr = enum_to_odr((ODR) (m_gyro_ctrl >> 4));

    switch ((m_accel_ctrl & 0b00001100) >> 2) {
        case A_FS_2G:
            accel_fs = 2;
            break;
        case A_FS_4G:
            accel_fs = 4;
            break;
        case A_FS_8G:
            accel_fs = 8;
            break;
        case A_FS_16G:
            accel_fs = 16;
            break;
    }

    switch (m_accel_ctrl & 0b00000011) {
        case A_BW_400HZ:
            accel_bw = 400;
            break;
        case A_BW_200HZ:
            accel_bw = 200;
            break;
        case A_BW_100HZ:
            accel_bw = 100;
            break;
        case A_BW_50HZ:
            accel_bw = 50;
            break;
    }

    switch ((m_gyro_ctrl & 0b00001110) >> 1) {
        case G_FS_125DPS:
            gyro_fs = 125;
            break;
        case G_FS_250DPS:
            gyro_fs = 250;
            break;
        case G_FS_500DPS:
            gyro_fs = 500;
            break;
        case G_FS_1000DPS:
            gyro_fs = 1000;
            break;
        case G_FS_2000DPS:
            gyro_fs = 2000;
            break;
    }

    cout << "Accel ODR: "   << accel_odr << " Hz"  << endl;
    cout << "Accel FS: +/-" << accel_fs  << " g"   << endl;
    cout << "Accel BW: "    << accel_bw  << " Hz"  << endl;
    cout << "Gyro ODR: "    << gyro_odr  << " Hz"  << endl;
    cout << "Gyro FS: "     << gyro_fs   << " dps" << endl;
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