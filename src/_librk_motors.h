#pragma once

#include <stdint.h>

#include "roboruka.h"

namespace rk {

class Motors {
public:
    Motors();
    ~Motors();

    void setup(const rkConfig& cfg);

    void set(int8_t left, int8_t right);
    void joystick(int32_t x, int32_t y);

private:
    Motors(const Motors&) = delete;

    int32_t scale(int32_t val);

    uint8_t m_id_left;
    uint8_t m_id_right;
    bool m_polarity_switch_left;
    bool m_polarity_switch_right;
};

}; // namespace rk
