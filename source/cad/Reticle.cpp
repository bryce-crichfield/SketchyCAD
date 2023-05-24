#include <cad/Reticle.h>

namespace Cad {
void Reticle::Draw(Core::Controller& controller, Core::Vector2 screen_point)
{
    auto& graphics = controller.GetGraphics();
    float x = screen_point.x;
    float y = screen_point.y;
    graphics.DrawLine(Core::Color::WHITE, x - 10, y, x - 3, y);
    graphics.DrawLine(Core::Color::WHITE, x + 3, y, x + 10, y);
    graphics.DrawLine(Core::Color::WHITE, x, y - 10, x, y - 3);
    graphics.DrawLine(Core::Color::WHITE, x, y + 3, x, y + 10);
}

void GridpointReticle::Draw(Core::Controller& controller, Core::Vector2 screen_point)
{
    Reticle::Draw(controller, screen_point);
    auto& graphics = controller.GetGraphics();
    auto screen_cursor = screen_point;
    float topleft_x = screen_cursor.x - 2;
    float topleft_y = screen_cursor.y - 2;
    float bottomright_x = screen_cursor.x + 2;
    float bottomright_y = screen_cursor.y + 2;

    graphics.DrawLine(Core::Color::WHITE, topleft_x, topleft_y, topleft_x + 1, topleft_y);
    graphics.DrawLine(Core::Color::WHITE, topleft_x, topleft_y, topleft_x, topleft_y + 1);
    graphics.DrawLine(Core::Color::WHITE, bottomright_x, bottomright_y, bottomright_x - 1, bottomright_y);
    graphics.DrawLine(Core::Color::WHITE, bottomright_x, bottomright_y, bottomright_x, bottomright_y - 1);

    graphics.DrawLine(Core::Color::WHITE, topleft_x, bottomright_y, topleft_x + 1, bottomright_y);
    graphics.DrawLine(Core::Color::WHITE, topleft_x, bottomright_y, topleft_x, bottomright_y - 1);
    graphics.DrawLine(Core::Color::WHITE, bottomright_x, topleft_y, bottomright_x - 1, topleft_y);
    graphics.DrawLine(Core::Color::WHITE, bottomright_x, topleft_y, bottomright_x, topleft_y + 1);
}

void MidpointReticle::Draw(Core::Controller& controller, Core::Vector2 screen_point)
{
    Reticle::Draw(controller, screen_point);

    auto& graphics = controller.GetGraphics();
    graphics.DrawLine(Core::Color::GREEN, screen_point.x - 3, screen_point.y - 3, screen_point.x, screen_point.y + 3);
    graphics.DrawLine(Core::Color::GREEN, screen_point.x, screen_point.y + 3, screen_point.x + 3, screen_point.y - 3);
    graphics.DrawLine(
        Core::Color::GREEN, screen_point.x + 3, screen_point.y - 3, screen_point.x - 3, screen_point.y - 3);
}

void EndpointReticle::Draw(Core::Controller& controller, Core::Vector2 screen_point)
{
    Reticle::Draw(controller, screen_point);

    auto& graphics = controller.GetGraphics();
    graphics.DrawRect(Core::Color::GREEN, screen_point.x - 3, screen_point.y - 3, 6, 6);
}

void IntersectionReticle::Draw(Core::Controller& controller, Core::Vector2 screen_point)
{
    Reticle::Draw(controller, screen_point);

    auto& graphics = controller.GetGraphics();
    // Draw an X at the intersection point
    graphics.DrawLine(
        Core::Color::GREEN, screen_point.x - 3, screen_point.y - 3, screen_point.x + 3, screen_point.y + 3);
    graphics.DrawLine(
        Core::Color::GREEN, screen_point.x + 3, screen_point.y - 3, screen_point.x - 3, screen_point.y + 3);
}
} // namespace Cad