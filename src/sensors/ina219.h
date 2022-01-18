#ifndef RAPID_CDH_INA219_H
#define RAPID_CDH_INA219_H

#include "../globals.h"

class Ina219 {
public:
    enum AddrSelect {
        GND = 0,
        VCC = 1,
        SDA = 2,
        SCL = 3
    };

    enum ShuntVoltageRangeSetting {
        PlusMinus_320_mV,
        PlusMinus_160_mV,
        PlusMinus_80_mV,
        PlusMinus_40_mV
    };

    // if the parameter for oversampling is n, the amount of oversampling done by the ADC is 2^n (e.g, n = 2 means 4 samples are averaged)
    // bus voltage full range is 32V, alternative is 16V
    Ina219(
        const char* device, AddrSelect addr0, AddrSelect addr1, double shuntResistance,
        ShuntVoltageRangeSetting v = PlusMinus_40_mV, int oversampling = 0, bool busVoltageFullRange = true);
    Status init();

    Status getShuntVoltage_mV(double& value);
    // supply voltage BEFORE the sensing resistor; exceeds the value observed by the component over which current is measured
    Status getSupplyVoltage_mV(double& value);
    // supply voltage AFTER the sensing resistor; less than the battery / supply voltage
    Status getBusVoltage_mV(double& value);

    Status getCurrent_mA(double& value);
    Status getPower_mW(double& value);

    Status modifyShunt(double newShuntResistance);
    Status reconfigure(ShuntVoltageRangeSetting v, int oversampling, bool busVoltageFullRange);
    Status setRunning(bool running);

    // causes loss of any configuration or calibration settings
    Status resetDevice();
};

#endif