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

    enum Registers {
        CONFIGURATION = 0,
        SHUNT_VOLTAGE = 1,
        BUS_VOLTAGE = 2,
        POWER = 3,
        CURRENT = 4,
        CALIBRATION = 5
    };

    // if the parameter for oversampling is n, the amount of oversampling done by the ADC is 2^n (e.g, n = 2 means 4 samples are averaged)
    // bus voltage full range is 32V, alternative is 16V
    INA219(
        const char* device, AddrSelect addr0, AddrSelect addr1, double shuntResistance,
        ShuntVoltageRangeSetting v = PlusMinus_40_mV, int oversampling = 0, bool busVoltageFullRange = true);
    [[nodiscard]] Status init();

    [[nodiscard]] Status getShuntVoltage_mV(double& value);
    [[nodiscard]] Status getShuntVoltageRaw(uint16_t& value);
    double parseShuntVoltage_mV(uint16_t raw);
    // supply voltage BEFORE the sensing resistor; exceeds the value observed by the component over which current is measured
    [[nodiscard]] Status getSupplyVoltage_mV(double& value);
    // supply voltage AFTER the sensing resistor; less than the battery / supply voltage
    [[nodiscard]] Status getBusVoltage_mV(double& value);
    [[nodiscard]] Status getBusVoltageRaw(uint16_t& value);
    double parseBusVoltage_mV(uint16_t raw);

    [[nodiscard]] Status getCurrent_mA(double& value);
    [[nodiscard]] Status getCurrentRaw(uint16_t& value);
    double parseCurrent_mA(uint16_t raw);
    // supply power is the amount drawn from the batteries; exceeds the power received by the component over which measurements are being taken
    [[nodiscard]] Status getSupplyPower_mW(double& value);
    // bus power is the amount used by the component over which measurements are being taken; less than the power drawn from the batteries / supply
    [[nodiscard]] Status getBusPower_mW(double& value);
    [[nodiscard]] Status getBusPowerRaw(uint16_t& value);
    double parseBusPower_mW(uint16_t raw);

    [[nodiscard]] Status modifyShunt(double newShuntResistance);
    [[nodiscard]] Status reconfigure(ShuntVoltageRangeSetting v, int oversampling, bool busVoltageFullRange);
    [[nodiscard]] Status setRunning(bool running);

    // causes loss of any configuration or calibration settings
    [[nodiscard]] Status resetDevice();

private:
    int fd;
    Status ctrRes;

    double shuntResistance;
    double currentLSB;
    double powerLSB;
    int lastCalibInt;

    ShuntVoltageRangeSetting voltageRangeSetting;
    int oversampling;
    int running; // 0 = false, 1 = true, -1 = unknown due to error
    bool busVoltageFullRange;
    int lastConfigInt;
};

#endif