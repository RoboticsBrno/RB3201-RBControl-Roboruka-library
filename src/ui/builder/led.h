#pragma once

#include "widget.h"
#include "../widgets/led.h"

namespace rkui {
namespace builder {

class Led : public Widget {
    friend class rkui::_Ui;
public:
    Led& color(const char *color) {
        extra().set("color", color);
        return *this;
    }

    Led& on(bool on) {
        extra().set("on", on);
        return *this;
    }

    Led& css(const char *name, const char *value) {
        Widget::css(name, value);
        return *this;
    }

    rkui::Led finish() { 
        return rkui::Led(m_uuid, extra().getBool("on", false));
    }

private:
    Led(uint16_t uuid, float x, float y, float w, float h, const char *color) :
        Widget("Led", uuid, x, y, w, h) {
        this->color(color);
    }
};

};
};
