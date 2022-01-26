#include "INA219.h"

#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "../globals.h"

int configInt(INA219::ShuntVoltageRangeSetting v, int os, bool fr, bool on) {
    int out = 0; // reset bit 15 = 0, unused bit 14 = 0
    out += fr << 13; // set bit 13 to zero iff the full voltage range of the ina219 is desired
    out += v << 11; // sets bits 11 and 12 to the appropriate values for the selected PGA gain/range
    out += (os > 0 ? (os + 8) : (os + 3)) << 7; // sets bits 7-10 to set the bus ADC to the desired oversampling level
    out += (os > 0 ? (os + 8) : (os + 3)) << 3; // sets bits 3-6 to set the shunt ADC to the desired oversampling level
    out += 0b111 * on; // sets bits 0-2 to set the ADC to continuously on mode or fully off mode
    return out;
}

int calibInt(double shuntResistance, INA219::ShuntVoltageRangeSetting vsetting, double& Ilsb, double& Plsb) {
    double maxV;
    switch (vsetting)
    {
    case INA219::PlusMinus_40_mV:
        maxV = 0.04;
        break;
    case INA219::PlusMinus_80_mV:
        maxV = 0.08;
        break;
    case INA219::PlusMinus_160_mV:
        maxV = 0.16;
        break;
    case INA219::PlusMinus_320_mV:
        maxV = 0.32;
        break;
    default:
        maxV = 0.32;
        break;
    }
    double maxCurrent = maxV / shuntResistance;
    Ilsb = maxCurrent / (1 << 15);
    Plsb = 20 * Ilsb;
    return static_cast<int>(0.04096 / (Ilsb * shuntResistance));
}

INA219::INA219(
        const char* device, AddrSelect addr0, AddrSelect addr1, double shuntResistance,
        ShuntVoltageRangeSetting v, int oversampling, bool busVoltageFullRange
) {
    this->voltageRangeSetting = v;
    this->busVoltageFullRange = busVoltageFullRange;
    this->oversampling = oversampling;
    this->shuntResistance = shuntResistance;
    this->running = true;
    fd = wiringPiI2CSetupInterface(device, 0b1000000 + addr0 + 4 * addr1);
    if (fd < 0) {
        ctrRes = I2C_SETUP_FAILURE;
        return;
    }
    lastConfigInt = configInt(v, oversampling, busVoltageFullRange, true);
    int confStatus = wiringPiI2CWriteReg16(fd, CONFIGURATION, lastConfigInt);
    lastCalibInt = calibInt(shuntResistance, v, currentLSB, powerLSB);
    int calStatus = wiringPiI2CWriteReg16(fd, CALIBRATION, lastCalibInt);
    if (confStatus < 0 || calStatus < 0) {
        if (calStatus < 0) {
            lastCalibInt = -1;
            currentLSB = -1;
            powerLSB = -1;
        }
        if (confStatus < 0) {
            lastConfigInt = -1;
        }
        ctrRes = I2C_WRITE_FAILURE;
    }
    ctrRes = SUCCESS;
}

Status INA219::init() {
    return ctrRes;
}

Status INA219::getShuntVoltage_mV(double& value) {
    if (fd == -1) {
        return I2C_SETUP_FAILURE;
    }
    int res = wiringPiI2CReadReg16(fd, SHUNT_VOLTAGE);
    if (res < 0) {
        return I2C_READ_FAILURE;
    }
    value = *((int16_t*) &res) / 100.0; // data processing casts
    return SUCCESS;
}

Status INA219::getShuntVoltageRaw(uint16_t& value) {
    if (fd == -1) {
        return I2C_SETUP_FAILURE;
    }
    int res = wiringPiI2CReadReg16(fd, SHUNT_VOLTAGE);
    if (res < 0) {
        return I2C_READ_FAILURE;
    }
    value = static_cast<uint16_t>(res); // overflow not possible, value obtained from function that reads from a two-byte register
    return SUCCESS;
}

double INA219::parseShuntVoltage_mV(uint16_t raw) {
    return *((int16_t*) &raw) / 100.0; // data processing casts
}

Status INA219::getSupplyVoltage_mV(double& value) {
    if (fd == -1) {
        return I2C_SETUP_FAILURE;
    }
    double bus = 0;
    double shunt = 0;
    Status s1 = getBusVoltage_mV(bus);
    Status s2 = getShuntVoltage_mV(shunt);
    if (s1 == I2C_READ_FAILURE || s2 == I2C_READ_FAILURE) {
        return I2C_READ_FAILURE;
    }
    value = bus + shunt;
    return SUCCESS;
}

Status INA219::getBusVoltage_mV(double& value) {
    if (fd == -1) {
        return I2C_SETUP_FAILURE;
    }
    int res = wiringPiI2CReadReg16(fd, BUS_VOLTAGE);
    if (res < 0) {
        return I2C_READ_FAILURE;
    }
    value = 4.0 * (res >> 3);
    return SUCCESS;
}

Status INA219::getBusVoltageRaw(uint16_t& value) {
    if (fd == -1) {
        return I2C_SETUP_FAILURE;
    }
    int res = wiringPiI2CReadReg16(fd, BUS_VOLTAGE);
    if (res < 0) {
        return I2C_READ_FAILURE;
    }
    value = static_cast<uint16_t>(res); // overflow not possible, value obtained from function that reads from a two-byte register
    return SUCCESS;
}

double INA219::parseBusVoltage_mV(uint16_t raw) {
    return 4.0 * (raw >> 3);
}

Status validateData(int fd) {
    int res = wiringPiI2CReadReg16(fd, INA219::BUS_VOLTAGE);
    return res < 0 ? I2C_READ_FAILURE : res & 1 ? I2C_BAD_DATA : SUCCESS;
}

Status INA219::getCurrent_mA(double& value) {
    if (fd == -1) {
        return I2C_SETUP_FAILURE;
    }
    if (currentLSB < 0) {
        return I2C_PRIOR_WRITE_FAILURE;
    }
    int res = wiringPiI2CReadReg16(fd, CURRENT);
    if (res < 0) {
        return I2C_READ_FAILURE;
    }
    Status valid = validateData(fd);
    if (valid != SUCCESS) {
        return valid;
    }
    value = *((int16_t*) &res) * currentLSB; // data processing casts
    return SUCCESS;
}

Status INA219::getCurrentRaw(uint16_t& value) {
    if (fd == -1) {
        return I2C_SETUP_FAILURE;
    }
    if (currentLSB < 0) {
        return I2C_PRIOR_WRITE_FAILURE;
    }
    int res = wiringPiI2CReadReg16(fd, CURRENT);
    if (res < 0) {
        return I2C_READ_FAILURE;
    }
    Status valid = validateData(fd);
    if (valid != SUCCESS) {
        return valid;
    }
    value = static_cast<uint16_t>(res); // overflow not possible, value obtained from function that reads from a two-byte register
    return SUCCESS;
}

double INA219::parseCurrent_mA(uint16_t raw) {
    return *((int16_t*) &raw) * currentLSB; // data processing casts
}

Status INA219::getSupplyPower_mW(double& value) {
    if (fd == -1) {
        return I2C_SETUP_FAILURE;
    }
    double voltage = 0;
    double current = 0;
    Status s1 = getSupplyVoltage_mV(voltage);
    Status s2 = getCurrent_mA(current);
    if (s2 == I2C_PRIOR_WRITE_FAILURE) {
        return I2C_PRIOR_WRITE_FAILURE;
    }
    if (s1 == I2C_READ_FAILURE || s2 == I2C_READ_FAILURE) {
        return I2C_READ_FAILURE;
    }
    value = voltage * current / 1000;
    return SUCCESS;
}

Status INA219::getBusPower_mW(double& value) {
    if (fd == -1) {
        return I2C_SETUP_FAILURE;
    }
    if (powerLSB < 0) {
        return I2C_PRIOR_WRITE_FAILURE;
    }
    int res = wiringPiI2CReadReg16(fd, POWER);
    if (res < 0) {
        return I2C_READ_FAILURE;
    }
    Status valid = validateData(fd);
    if (valid != SUCCESS) {
        return valid;
    }
    value = res * powerLSB;
    return SUCCESS;
}

Status INA219::getBusPowerRaw(uint16_t& value) {
    if (fd == -1) {
        return I2C_SETUP_FAILURE;
    }
    if (powerLSB < 0) {
        return I2C_PRIOR_WRITE_FAILURE;
    }
    int res = wiringPiI2CReadReg16(fd, POWER);
    if (res < 0) {
        return I2C_READ_FAILURE;
    }
    Status valid = validateData(fd);
    if (valid != SUCCESS) {
        return valid;
    }
    value = static_cast<uint16_t>(res); // overflow not possible, value obtained from function that reads from a two-byte register
    return SUCCESS;
}

double INA219::parseBusPower_mW(uint16_t raw) {
    return raw * powerLSB;
}

Status INA219::modifyShunt(double newShuntResistance) {
    if (fd == -1) {
        return I2C_SETUP_FAILURE;
    }
    if (lastConfigInt < 0) {
        return I2C_PRIOR_WRITE_FAILURE;
    }
    int i = calibInt(newShuntResistance, voltageRangeSetting, currentLSB, powerLSB);
    if (i == lastCalibInt) {
        return SUCCESS;
    }
    int calStatus = wiringPiI2CWriteReg16(fd, CALIBRATION, i);
    if (calStatus < 0) {
        lastCalibInt = -1;
        currentLSB = -1;
        powerLSB = -1;
        return I2C_WRITE_FAILURE;
    } else {
        lastCalibInt = i;
        return SUCCESS;
    }
}

Status INA219::reconfigure(ShuntVoltageRangeSetting v, int oversampling, bool busVoltageFullRange) {
    if (fd == -1) {
        return I2C_SETUP_FAILURE;
    }
    if (running < 0) {
        return I2C_PRIOR_WRITE_FAILURE;
    }
    int i = configInt(v, oversampling, busVoltageFullRange, running);
    if (i == lastConfigInt) {
        return SUCCESS;
    }
    int confStatus = wiringPiI2CWriteReg16(fd, CONFIGURATION, i);
    if (confStatus < 0) {
        lastConfigInt = -1;
        return I2C_WRITE_FAILURE;
    } else {
        lastConfigInt = i;
        return SUCCESS;
    }
}

Status INA219::setRunning(bool running) {
    if (fd == -1) {
        return I2C_SETUP_FAILURE;
    }
    if (lastConfigInt < 0) {
        return I2C_PRIOR_WRITE_FAILURE;
    }
    int i = configInt(voltageRangeSetting, oversampling, busVoltageFullRange, running);
    if (i == lastConfigInt) {
        return SUCCESS;
    }
    if (lastConfigInt < 0) {
        return I2C_PRIOR_WRITE_FAILURE;
    }
    int confStatus = wiringPiI2CWriteReg16(fd, CONFIGURATION, i);
    if (confStatus < 0) {
        lastConfigInt = -1;
        return I2C_WRITE_FAILURE;
    } else {
        lastConfigInt = i;
        return SUCCESS;
    }
}

Status INA219::resetDevice() {
    if (fd == -1) {
        return I2C_SETUP_FAILURE;
    }
    int confStatus = wiringPiI2CWriteReg16(fd, CONFIGURATION, 1 << 15);
    if (confStatus < 0) {
        lastConfigInt = -1;
        lastCalibInt = -1;
        running = -1;
        return I2C_WRITE_FAILURE;
    } else {
        lastConfigInt = 0x399F;
        lastCalibInt = 0x0000;
        running = true;
        busVoltageFullRange = true;
        voltageRangeSetting = PlusMinus_320_mV;
        oversampling = 0;
        // recalibration is required; return 0 from these registers instead until the sensor is recalibrated
        currentLSB = 0;
        powerLSB = 0;
        return SUCCESS;
    }
}