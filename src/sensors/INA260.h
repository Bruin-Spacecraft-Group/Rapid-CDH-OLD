#ifndef RAPIDCDH_INA260_H
#define RAPIDCDH_INA260_H

#include <cstdint>

#include "../globals.h"

class INA260 {
public:
    enum RegAddr: uint8_t;

    INA260(const char* device, uint8_t dev_id);

    [[nodiscard]] Status init();
    [[nodiscard]] Status write_reg_16(RegAddr reg, uint16_t data) const;
    [[nodiscard]] Status read_reg_16(RegAddr reg, uint16_t &data) const;

    [[nodiscard]] Status reset() const;

    // Read raw sensor output
    [[nodiscard]] Status read_current();

    // Return actual sensor values
    void get_current(float &current) const;

    enum RegAddr: uint8_t {
        REG_CONFIG          = 0x00,
        REG_CURRENT         = 0x01,
        REG_BUS_VOLTAGE     = 0x02,
        REG_POWER           = 0x03,
        REG_MASK_ENABLE     = 0x06,
        REG_ALERT_LIMIT     = 0x07,
        REG_MANUFACTURER_ID = 0xFE,
        REG_DIE_ID          = 0xFF
    };

private:
    const char* m_i2c_dev;
    uint8_t m_dev_id;
    int32_t m_i2c;

    // Raw sensor outputs
    uint16_t m_current;

    // Sensor sensitivity
    static constexpr float CURRENT_SENS = 1.25; // mA/LSB
};

#endif
