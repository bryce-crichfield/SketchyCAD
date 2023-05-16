#include <Cad.h>
namespace Cad {

void CameraController::Pan(int dx, int dy, float grid_size)
{
    float grid_scale = camera.scale * grid_size;
    camera.pan_x += dx * grid_scale;
    camera.pan_y += dy * grid_scale;
}

void CameraController::ZoomIn(Controller& controller, float grid_size) { 
    camera.scale += grid_size; 
    
    // Push pan back to center
    auto& graphics = controller.GetGraphics();
    float width = graphics.GetWidth();
    float height = graphics.GetHeight();

    float pan_x = camera.pan_x;
    float pan_y = camera.pan_y;

    float pan_x_max = width * 0.5f + grid_size * 0.5f;
    float pan_x_min = width * 0.5f - grid_size * 0.5f;

    float pan_y_max = height * 0.5f + grid_size * 0.5f;
    float pan_y_min = height * 0.5f - grid_size * 0.5f;

    if (pan_x > pan_x_max) {
        camera.pan_x = pan_x_max;
    } else if (pan_x < pan_x_min) {
        camera.pan_x = pan_x_min;
    }

    if (pan_y > pan_y_max) {
        camera.pan_y = pan_y_max;
    } else if (pan_y < pan_y_min) {
        camera.pan_y = pan_y_min;
    }




}

void CameraController::ZoomOut(Controller& controller, float grid_size) {
    camera.scale -= grid_size; 

        // Push pan back to center
    auto& graphics = controller.GetGraphics();
    float width = graphics.GetWidth();
    float height = graphics.GetHeight();

    float pan_x = camera.pan_x;
    float pan_y = camera.pan_y;

    float pan_x_max = width * 0.5f + grid_size * 0.5f;
    float pan_x_min = width * 0.5f - grid_size * 0.5f;

    float pan_y_max = height * 0.5f + grid_size * 0.5f;
    float pan_y_min = height * 0.5f - grid_size * 0.5f;

    if (pan_x > pan_x_max) {
        camera.pan_x = pan_x_max;
    } else if (pan_x < pan_x_min) {
        camera.pan_x = pan_x_min;
    }

    if (pan_y > pan_y_max) {
        camera.pan_y = pan_y_max;
    } else if (pan_y < pan_y_min) {
        camera.pan_y = pan_y_min;
    }   
}

void CameraController::Zero(Graphics& graphics, float grid_size)
{
    camera.pan_x = graphics.GetWidth() * 0.5f;
    camera.pan_y = graphics.GetHeight() * 0.5f;
    camera.scale = 1.0f / grid_size;
}

void CameraController::Update(Controller& controller)
{
    if (bypass) return;
    float grid_size = controller.GetGridSize();
    auto& input = controller.GetInput();
    auto& chrono = controller.GetChrono();
    auto& graphics = controller.GetGraphics();

    if (input.IsPressed(Key::Up)) {
        Pan(0, -1, grid_size);
    }

    if (input.IsPressed(Key::Down)) {
        Pan(0, 1, grid_size);
    }

    if (input.IsPressed(Key::Left)) {
        Pan(-1, 0, grid_size);
    }

    if (input.IsPressed(Key::Right)) {
        Pan(1, 0, grid_size);
    }

    if (input.IsPressed(Key::PageUp)) {
        // Save pan info before zooming
        Vector2 pan = {camera.pan_x, camera.pan_y};
        ZoomIn(controller, grid_size);
    }

    if (input.IsPressed(Key::PageDown)) {
        ZoomOut(controller, grid_size);
    }

    unsigned max_zoom = 256;
    // finding largest multiple of grid_size that is less than max_zoom
    float max_scale = std::floor(max_zoom / grid_size) * grid_size;
    std::cout << max_scale << std::endl;
    camera.scale = std::clamp(camera.scale, grid_size, max_scale);
}

void CameraController::SetBypass(bool bypass) { this->bypass = bypass; }

Camera& CameraController::GetCamera() { return camera; }
} // namespace Cad