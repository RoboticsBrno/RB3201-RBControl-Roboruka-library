#include <stdio.h>

#include "ui.h"
#include "rbwebserver.h"

namespace rkui {

_Ui UI;

_Ui::_Ui() : m_uuid_counter(1) {

}

_Ui::~_Ui() {
}

void _Ui::finish() {
    auto *layout = new rbjson::Object;
    auto *widgets = new rbjson::Array;

    layout->set("widgets", widgets);
    layout->set("cols", 12);
    layout->set("rows", 18);
    layout->set("enableSplitting", new rbjson::Bool(true));

    for(auto& w : m_widgets) {
        m_callbacks.insert(w->callbacks().begin(), w->callbacks().end());
        widgets->push_back(w->serializeAndDestroy());
        delete w.release();
    }
    m_widgets.clear();
    m_widgets.shrink_to_fit();

    {
        const auto layout_json = layout->str();
        esp_err_t e = rb_web_add_file("layout.json", layout_json.c_str(), layout_json.size());
    }

    delete layout;
}

void _Ui::onEvent(rbjson::Object *pkt) {
    auto src = pkt->getString("src");
    const auto cb = m_callbacks.find(src);
    if(cb != m_callbacks.end()) {
        cb->second();
    }
}

};
