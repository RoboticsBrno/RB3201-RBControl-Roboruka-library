#pragma once

#include <atomic>

#include "RBControl_arm.hpp"
#include "rbprotocol.h"

#include "_librk_arm.h"
#include "_librk_motors.h"

namespace rk {

class Context {
public:
    Context();
    ~Context();

    void setup(const rkConfig& cfg);

    rb::Protocol *prot() const { return m_prot; }
    ArmWrapper &arm() { return m_arm; }
    Motors &motors() { return m_motors; }

private:
    void handleRbcontrollerMessage(const std::string& cmd, rbjson::Object* pkt);

    ArmWrapper m_arm;
    Motors m_motors;
    rb::Protocol *m_prot;
    rb::Protocol::callback_t m_prot_callback;

    std::atomic<bool> m_initialized;
};

extern Context gCtx;

};
