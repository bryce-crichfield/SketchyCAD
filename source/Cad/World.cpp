#include <Cad.h>

namespace Cad
{
 
void DottedLineVertical(Graphics& graphics, float n, Vector2 range, unsigned stride) {
    for (int i = 0; i < n; i += stride) {
        graphics.DrawLine(Color::WHITE, i, range.x, i, range.y);
    }
}

void DottedLineHorizontal(Graphics& graphics, float n, Vector2 range, unsigned stride) {
    for (int i = 0; i < n; i += stride) {
        graphics.DrawLine(Color::WHITE, range.x, i, range.y, i);
    }
}

void World::draw_grid(Controller& controller)
{
    // We need to get the grid basis vector in world space
    // Then get it in screen space
    Transform view_transform = controller.GetCamera().GetTransform();
    Camera& camera = controller.GetCamera();
    Graphics &graphics = controller.GetGraphics();
    float grid_size_screen = controller.GetGridSize() * view_transform.scale;
    Vector2 screen_center(graphics.GetWidth() / 2, graphics.GetHeight() / 2);
    // Draw first quadrant of dots
    for (int i = screen_center.x; i > 0; i -= grid_size_screen) {
        for (int j = screen_center.y; j > 0; j -= grid_size_screen) {
            graphics.SetPixel(Color::GRAY, i, j);
        }
    }

    // Draw second quadrant of dots
    for (int i = screen_center.x; i < graphics.GetWidth(); i += grid_size_screen) {
        for (int j = screen_center.y; j > 0; j -= grid_size_screen) {
            graphics.SetPixel(Color::GRAY, i, j);
        }
    }

    // Draw third quadrant of dots
    for (int i = screen_center.x; i > 0; i -= grid_size_screen) {
        for (int j = screen_center.y; j < graphics.GetHeight(); j += grid_size_screen) {
            graphics.SetPixel(Color::GRAY, i, j);
        }
    }

    // Draw fourth quadrant of dots
    for (int i = screen_center.x; i < graphics.GetWidth(); i += grid_size_screen) {
        for (int j = screen_center.y; j < graphics.GetHeight(); j += grid_size_screen) {
            graphics.SetPixel(Color::GRAY, i, j);
        }
    }
}

void World::draw_objects(Controller& controller)
{
    Transform view_transform = controller.GetCamera().GetTransform();
    controller.GetGraphics().PushTransform(view_transform);
    for (auto &object : objects)
    {
        object.second->Update(controller);
    }
    controller.GetGraphics().PopTransform();
}

void World::draw_origin(Controller& controller)
{
    Transform view_transform = controller.GetCamera().GetTransform();
    float scale = view_transform.scale;
    float pan_x = view_transform.x + (scale / view_transform.x);
    float pan_y = view_transform.y + (scale / view_transform.y);

    Graphics& graphics = controller.GetGraphics();
    graphics.DrawLine(Color::RED, pan_x, 0, pan_x, graphics.GetHeight());
    graphics.DrawLine(Color::GREEN, 0, pan_y, graphics.GetWidth(), pan_y);
}

void World::draw_cursor(Controller& controller)
{

}

World::World()
{
    for (unsigned i = 0; i < MAX_OBJECTS; i++)
    {
        unique_id_queue.push(i);
    }
}

void World::Update(Controller& controller)
{
    draw_grid(controller);
    draw_objects(controller);
    draw_origin(controller);
    draw_cursor(controller);
}

void World::Register(unsigned id, std::unique_ptr<IObject> object)
{
    // if there is a collision, do not register the object
    if (objects.find(id) != objects.end())
    {
        std::cout << "voiding registration of object with id: " << id << std::endl;
        return;
    }


    std::cout << "registering " << object->ToString() << " with id: " << id << std::endl;
    // if the id is not in use, register the object
    objects[id] = std::move(object);
}

std::unique_ptr<IObject> World::Remove(unsigned id)
{
    if (objects.find(id) != objects.end())
    {
        std::unique_ptr<IObject> object = std::move(objects[id]);
        std::cout << "removing " << object->ToString() << " with id: " << id << std::endl;
        objects.erase(id);
        unique_id_queue.push(id);
        return object;
    }
}
} // namespace Cad