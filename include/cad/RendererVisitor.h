#pragma once

#include <cad/object/ObjectVisitor.h>
#include <core/Core.h>

namespace Cad {
struct RendererVisitor : public ObjectVisitor {
    Core::Graphics& graphics;
    RendererVisitor(Core::Graphics& graphics) : graphics(graphics) {}

    void Visit(LineObject& object) override {
        auto start = object.start;
        auto end = object.end;
        auto color = object.IsSelected() ? Core::Color::RED : Core::Color::WHITE;
        graphics.DrawLine(color, start.x, start.y, end.x, end.y);
    }

    void Visit(CircleObject& object) override {
        auto& center = object.center;
        auto radius = object.radius;
        auto color = object.IsSelected() ? Core::Color::RED : Core::Color::WHITE;

        graphics.DrawCircle(color, center.x, center.y, radius);
    }

    void Visit(PolylineObject& object) override {}
};
} // namespace Cad