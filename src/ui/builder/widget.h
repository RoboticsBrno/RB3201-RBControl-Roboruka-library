#pragma once

#include <memory>
#include <string>
#include <functional>

#include <stdint.h>

#include "rbjson.h"

namespace rkui {

class _Ui;

namespace builder {

class Widget {
    friend class rkui::_Ui;
public:
    Widget(Widget&& o) noexcept;
    virtual ~Widget();

protected:
    Widget(const char *type, uint16_t uuid, float x, float y, float w, float h);

    void css(const char *key, const char *value);

    void addCallback(const char *name, std::function<void()> cb);

    rbjson::Object& extra();
    rbjson::Object& style();

    uint16_t m_uuid;

private:
    Widget(const Widget&) = delete;
    Widget& operator=(const Widget&) = delete;

    rbjson::Object *serializeAndDestroy();

    const std::unordered_map<std::string, std::function<void()> >& callbacks() const {
        return m_callbacks;
    }

    const char *m_type;
    float m_x, m_y;
    float m_w, m_h;

    std::unique_ptr<rbjson::Object> m_extra;
    rbjson::Object *m_style;

    std::unordered_map<std::string, std::function<void()> > m_callbacks;
};

};
};
