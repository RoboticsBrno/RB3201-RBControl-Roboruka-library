#pragma once

#include "RBControl_arm.hpp"
#include "rbprotocol.h"

namespace rk {

class ArmWrapper {
public:
    ArmWrapper();
    ~ArmWrapper();

    void setup(const rkConfig& cfg);

    void sendInfo();
    bool moveTo(double x, double y);
    void setGrabbing(bool grab);
    bool isGrabbing() const;

private:
    ArmWrapper(const ArmWrapper&) = delete;

    rb::Arm *m_arm;
    std::vector<rb::Angle> m_bone_trims;
};

}; // namespace rk
