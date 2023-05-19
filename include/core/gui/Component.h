#pragma once

#include <core/math/Vector2.h>

namespace Core::Gui {
struct Component {
    Vector2 m_position;
    Vector2 m_size;

  public:
    Component(unsigned width, unsigned height) : m_position(0, 0), m_size(width, height) {}

    Vector2 GetPosition() const;
    Vector2 GetSize() const;
    void SetPosition(unsigned x, unsigned y);
    void SetSize(unsigned width, unsigned height);
};
}