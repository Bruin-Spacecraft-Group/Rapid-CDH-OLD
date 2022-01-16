#ifndef RAPIDCDH_UM7_H
#define RAPIDCDH_UM7_H

#include <cstdint>

#include "../globals.h"

class UM7 {
public:
    UM7(uint8_t channel, uint32_t speed);

    [[nodiscard]] Status init();

private:
    uint8_t m_channel;
    uint32_t m_speed;
    int32_t m_spi;
};


#endif //RAPIDCDH_UM7_H
