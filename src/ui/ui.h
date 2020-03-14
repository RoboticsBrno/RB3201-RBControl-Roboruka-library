#pragma once

#include <vector>
#include <memory>

#include "builder/button.h"
#include "builder/led.h"

namespace rk {
    class Context;
};

namespace rkui {

class _Ui {
    friend class rk::Context;
public:
    _Ui();
    ~_Ui();

    void clear();

    builder::Button& button(float x, float y, float w, float h, const char *text) {
        auto *widget = new builder::Button(m_uuid_counter++, x, y, w, h, text);
        m_widgets.push_back(std::unique_ptr<builder::Widget>(widget));
        return *widget;
    }

    builder::Led& led(float x, float y, float w, float h, const char *color) {
        auto *widget = new builder::Led(m_uuid_counter++, x, y, w, h, color);
        m_widgets.push_back(std::unique_ptr<builder::Led>(widget));
        return *widget;
    }

    void finish();

private:
    void onEvent(rbjson::Object *pkt);

    uint16_t m_uuid_counter;
    std::vector<std::unique_ptr<builder::Widget> > m_widgets;
    std::unordered_map<std::string, std::function<void()> > m_callbacks;
};

extern _Ui UI;
};
