#include <stdio.h>
#include <stdarg.h>

#include "roboruka.h"
#include "_librk_context.h"

#include "RBControl.hpp"

#define TAG "roboruka"

using namespace rb;
using namespace rk;
using namespace mcp3008;

void rkSetup(const rkConfig& cfg) {
    gCtx.setup(cfg);
}

void rkControllerSendLog(const char *format, ...) {
    if(gCtx.prot() == nullptr) {
        ESP_LOGE(TAG, "%s: protocol not initialized!", __func__);
        return;
    }
    va_list args;
    va_start(args, format);
    gCtx.prot()->send_log(format, args);
    va_end(args);
}

void rkControllerSend(const char *cmd, rbjson::Object *data) {
    if(gCtx.prot() == nullptr) {
        ESP_LOGE(TAG, "%s: protocol not initialized!", __func__);
        return;
    }
    gCtx.prot()->send(cmd, data);
}

void rkControllerSendMustArrive(const char *cmd, rbjson::Object *data) {
    if(gCtx.prot() == nullptr) {
        ESP_LOGE(TAG, "%s: protocol not initialized!", __func__);
        return;
    }
    gCtx.prot()->send_mustarrive(cmd, data);
}

bool rkArmMoveTo(double x, double y) {
    if(!gCtx.arm().moveTo(x, y)) {
        ESP_LOGE(TAG, "%s: can't move to %.1f %.1f, failed to solve the movement!", __func__, x, y);
        return false;
    }
    return true;
}

void rkArmSetGrabbing(bool grab) {
    return gCtx.arm().setGrabbing(grab);
}

bool rkArmIsGrabbing() {
    return gCtx.arm().isGrabbing();
}

float rkBatteryCoef() {
    return Manager::get().battery().fineTuneCoef();
}

uint32_t rkBatteryRaw() {
    return Manager::get().battery().raw();
}

uint32_t rkBatteryPercent() {
    return Manager::get().battery().pct();
}

uint32_t rkBatteryVoltageMv() {
    return Manager::get().battery().voltageMv();
}

void rkMotorsSetPower(int8_t left, int8_t right) {
    gCtx.motors().set(left, right);
}

void rkMotorsJoystick(int32_t x, int32_t y) {
    gCtx.motors().joystick(x, y);
}

void rkLedRed(bool on) {
    Manager::get().leds().red(on);
}

void rkLedYellow(bool on) {
    Manager::get().leds().yellow(on);
}

void rkLedGreen(bool on) {
    Manager::get().leds().green(on);
}

void rkLedBlue(bool on) {
    Manager::get().leds().blue(on);
}

void rkLedAll(bool on) {
    auto& l = Manager::get().leds();
    l.red(on);
    l.yellow(on);
    l.green(on);
    l.blue(on);
}

void rkLedById(uint8_t id, bool on) {
    auto& l = Manager::get().leds();
    switch(id) {
        case 1: l.red(on); break;
        case 2: l.yellow(on); break;
        case 3: l.green(on); break;
        case 4: l.blue(on); break;
    }
}

bool rkButtonIsPressed(uint8_t id) {
    if(id == 0) {
        ESP_LOGE(TAG, "%s: invalid id %d, buttons are indexed from 1, just like on the board (SW1, SW2...)!", __func__, id);
        return false;
    } else if(id > 3) {
        ESP_LOGE(TAG, "%s: maximum button id is 3, you are using %d!", __func__, id);
        return false;
    }

    int pin = SW1 + (id - 1);
    auto& exp = Manager::get().expander();
    for(int i = 0; i < 3; ++i) {
        const bool pressed = exp.digitalRead(pin) == 0;
        if(!pressed)
            return false;
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    return true;
}

void rkLineCalibrate(float motor_time_coef) {
    auto& man = Manager::get();

    const auto l = gCtx.motors().idLeft();
    const auto r = gCtx.motors().idRight();

    const auto maxleft = man.motor(l).pwmMaxPercent();
    const auto maxright = man.motor(r).pwmMaxPercent();

    constexpr int8_t pwr = 40;

    auto cal = gCtx.line().startCalibration();
    man.setMotors()
        .pwmMaxPercent(l, 100)
        .pwmMaxPercent(r, 100)
        .power(l, -pwr).power(r, pwr)
        .set();

    for(int i = 0; i < 30*motor_time_coef; ++i) {
        cal.record();
        vTaskDelay(pdMS_TO_TICKS(20));
    }

    man.setMotors().power(l, pwr).power(r, -pwr).set();
    for(int i = 0; i < 50*motor_time_coef; ++i) {
        cal.record();
        vTaskDelay(pdMS_TO_TICKS(20));
    }

    man.setMotors().power(l, -pwr).power(r, pwr).set();
    for(int i = 0; i < 30*motor_time_coef; ++i) {
        cal.record();
        vTaskDelay(pdMS_TO_TICKS(20));
    }

    man.setMotors()
        .power(l, 0).power(r, 0)
        .pwmMaxPercent(l, maxleft)
        .pwmMaxPercent(r, maxright)
        .set();

    cal.save();
    gCtx.saveLineCalibration();
}

void rkLineClearCalibration() {
    LineSensor::CalibrationData cal;
    for(int i = 0; i < Driver::CHANNELS; ++i) {
        cal.min[i] = 0;
        cal.range[i] = Driver::MAX_VAL;
    }
    gCtx.line().setCalibration(cal);
    gCtx.saveLineCalibration();
}

uint16_t rkLineGetSensor(uint8_t sensorId) {
    return gCtx.line().calibratedReadChannel(sensorId);
}

float rkLinePosition(bool white_line, uint8_t line_threshold_pct) {
    return gCtx.line().readLine(white_line, float(line_threshold_pct)/100.f);
}
