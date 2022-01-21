#ifndef RAPID_CDH_INA219_H
#define RAPID_CDH_INA219_H

#include <cstdint>

#include "../globals.h"

class INA219 {
public:
    enum AddrSelect {
        GND = 0,
        VCC = 1,
        SDA = 2,
        SCL = 3
    };

    enum ShuntVoltageRangeSetting {
        PlusMinus_40_mV = 0,
        PlusMinus_80_mV = 1,
        PlusMinus_160_mV = 2,
        PlusMinus_320_mV = 3
    };

    // if the parameter for oversampling is n, the amount of oversampling done by the ADC is 2^n (e.g, n = 2 means 4 samples are averaged)
    // bus voltage full range is 32V, alternative is 16V
    INA219(
        const char* device, AddrSelect addr0, AddrSelect addr1, double shuntResistance,
        ShuntVoltageRangeSetting v = PlusMinus_40_mV, int oversampling = 0, bool busVoltageFullRange = true);
    [[nodiscard]] Status init();

    [[nodiscard]] Status getShuntVoltage_mV(double& value);
    // supply voltage BEFORE the sensing resistor; exceeds the value observed by the component over which current is measured
    [[nodiscard]] Status getSupplyVoltage_mV(double& value);
    // supply voltage AFTER the sensing resistor; less than the battery / supply voltage
    [[nodiscard]] Status getBusVoltage_mV(double& value);

    [[nodiscard]] Status getCurrent_mA(double& value);
    [[nodiscard]] Status getPower_mW(double& value);

    [[nodiscard]] Status modifyShunt(double newShuntResistance);
    [[nodiscard]] Status reconfigure(ShuntVoltageRangeSetting v, int oversampling, bool busVoltageFullRange);
    [[nodiscard]] Status setRunning(bool running);

    // causes loss of any configuration or calibration settings
    [[nodiscard]] Status resetDevice();

private:
    int fd;
    Status ctrRes;

    int lastConfigInt;
    ShuntVoltageRangeSetting voltageRangeSetting;
    int oversampling;
    bool busVoltageFullRange;
    int running; // 0 = false, 1 = true, -1 = unknown due to error

    int lastCalibInt;
    double shuntResistance;
    double currentLSB;
    double powerLSB;
};

#endif