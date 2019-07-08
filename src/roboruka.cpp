#include <stdio.h>
#include <stdarg.h>

#include "roboruka.h"
#include "_librk_context.h"

#include "RBControl.hpp"

#define TAG "roboruka"

using namespace rb;
using namespace rk;

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

void rkBatterySetCoef(float coef) {
    Manager::get().battery().setCoef(coef);
}

float rkBatteryCoef() {
    return Manager::get().battery().coef();
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
