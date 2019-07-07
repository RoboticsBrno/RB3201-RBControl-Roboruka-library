#include <stdio.h>
#include "RBControl.hpp"
#include "RBControl_wifi.hpp"
#include "rbprotocol.h"
#include "rbwebserver.h"

#include "roboruka.h"
#include "_librk_context.h"

using namespace rb;

namespace rk {

Context gCtx;

Context::Context() {
    m_prot = nullptr;
}

Context::~Context() {

}

void Context::setup(const rkConfig& cfg) {
    bool expected = false;
    if(!m_initialized.compare_exchange_strong(expected, true)) {
        ESP_LOGE("roboruka", "rkSetup was called more than once, this is WRONG!");
        return;
    }

    // Initialize the robot manager
    auto& man = Manager::get();
    man.install();

    m_arm.setup(cfg);
    m_motors.setup(cfg);

    // Set the battery measurement coeficient
    auto& batt = man.battery();
    batt.setCoef(cfg.battery_coefficient);

    // Set-up servos
    auto& servos = man.initSmartServoBus(3);
    if(!servos.posOffline(2).isNaN())
        servos.setAutoStop(2);
    servos.limit(0,  0_deg, 220_deg);
    servos.limit(1, 85_deg, 210_deg);
    servos.limit(2, 75_deg, 160_deg);

    if(man.expander().digitalRead(SW1) != cfg.wifi_default_ap) {
        if(cfg.wifi_name && strlen(cfg.wifi_name) != 0) {
            man.leds().yellow();
            WiFi::connect(cfg.wifi_name, cfg.wifi_password);
        }
    } else {
        if(cfg.wifi_ap_password && strlen(cfg.wifi_ap_password) != 0) {
            man.leds().green();
            char ssid[32] = { 0 };
            snprintf(ssid, sizeof(ssid), "%s-%s", cfg.owner, cfg.name);
            WiFi::startAp(ssid, cfg.wifi_ap_password, cfg.wifi_ap_channel);
        }
    }

    if(cfg.rbcontroller_app_enable) {
        m_prot_callback = cfg.rbcontroller_message_callback;

        // Start web server with control page (see data/index.html)
        rb_web_start(80);

        // Initialize the communication protocol
        using namespace std::placeholders;
        m_prot = new Protocol(cfg.owner, cfg.name, "Compiled at " __DATE__ " " __TIME__,
            std::bind(&Context::handleRbcontrollerMessage, this, _1, _2));
        m_prot->start();
    }
}

void Context::handleRbcontrollerMessage(const std::string& cmd, rbjson::Object* pkt) {
    if(cmd == "arminfo") {
        m_arm.sendInfo();
    } else if(m_prot_callback) {
        m_prot_callback(cmd, pkt);
    }
}


}; // namespace rk
