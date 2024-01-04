#pragma once

#include <cad/RendererVisitor.h>
#include <cad/gui/Component.h>

namespace Cad {
struct Minimap : public Component {
    void OnRender(Cad::Controller& controller) override {
        float x = position.x;
        float y = position.y;
        float width = size.x;
        float height = size.y;
        auto transform = controller.GetViewfinder().GetViewTransform();
        auto& image_graphics = controller.GetGraphics();
        Core::Transform minimap_transform;
        minimap_transform.scale = transform.scale * 0.15;
        minimap_transform.x = (transform.x * 0.15) + ((x + width / 2.0f) / 2.0f);
        minimap_transform.y = (transform.y * 0.15) + ((y + height / 2.0f) / 2.0f);

        image_graphics.PushClip(x, y, width, height);
        image_graphics.FillRect(Core::Color::BLACK, x, y, width, height);
        image_graphics.PushTransform(minimap_transform);
        RendererVisitor minimap_renderer(image_graphics);
        controller.GetRegistry().VisitObjects(minimap_renderer);
        image_graphics.PopTransform();
        image_graphics.DrawRect(Core::Color::WHITE, x, y, width, height);
        image_graphics.PopClip();
    }
};
} // namespace Cad