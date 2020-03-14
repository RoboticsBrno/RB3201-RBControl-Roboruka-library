#pragma once

#include "RBControl_arm.hpp"
#include "rbprotocol.h"

#include "roboruka.h"

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

    bool getCurrentPosition(double &outX, double &outY) const;

private:
    ArmWrapper(const ArmWrapper&) = delete;

    rb::Arm *m_arm;
    std::vector<rb::Angle> m_bone_trims;
};

}; // namespace rk
