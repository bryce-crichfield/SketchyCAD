#pragma once

#include <core/gui/Panel.h>

namespace Core::Gui {
enum class Alignment {
    START,
    MIDDLE,
    END
};

class Panel;

class LayoutStrategy {
    Alignment alignment = Alignment::START;

  public:
    virtual void Layout(Panel& parent, Panel& child) = 0;
    virtual void Reset(unsigned x, unsigned y) = 0;

    Alignment GetAlignment();
    void SetAlignment(Alignment alignment);
};

class VerticalLayout : public LayoutStrategy {
    unsigned layout_x;
    unsigned layout_y;

  public:
    void Layout(Panel& parent, Panel& child) override;
    void Reset(unsigned x, unsigned y) override;
};

class HorizontalLayout : public LayoutStrategy {
    unsigned layout_x;
    unsigned layout_y;

  public:
    void Layout(Panel& parent, Panel& child) override;
    void Reset(unsigned x, unsigned y) override;
};
}