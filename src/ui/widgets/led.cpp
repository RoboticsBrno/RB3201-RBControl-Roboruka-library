#include "led.h"

#include "../../_librk_context.h"

namespace rkui {

void Led::setOn(bool on) {
    auto *prot = rk::gCtx.prot();
    if(!prot || m_on == on)
        return;

    m_on = on;
    
    auto *params = new rbjson::Object;
    params->set("id", m_uuid);
    params->set("on", new rbjson::Bool(on));
    prot->send_mustarrive("gev", params);
}

};
