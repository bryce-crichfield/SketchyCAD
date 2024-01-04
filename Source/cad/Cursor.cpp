#include "cad/Cursor.h"
namespace Cad {

struct SnapVisitor : public ObjectVisitor {
    std::vector<SnapVector> snap_vectors;
    Core::Vector2 mouse_pos;
    Core::Transform view_transform;
    float grid_size = 1;

    SnapVisitor(Core::Vector2 mouse_pos, Core::Transform view_transform, float grid_size)
        : mouse_pos(mouse_pos), view_transform(view_transform), grid_size(grid_size)
    {
    }

    void Visit(LineObject& object) override
    {
        // Check for the midpoint snap condition
        auto start_screen = view_transform.Apply(object.start);
        auto end_screen = view_transform.Apply(object.end);
        auto midpoint_screen = (start_screen + end_screen) / 2;
        auto distance_from_midpoint = (mouse_pos - midpoint_screen).Length();
        float grid_screen_size = grid_size * view_transform.scale;
        if (distance_from_midpoint < 10) {
            snap_vectors.push_back(SnapVector(midpoint_screen, ReticleType::Midpoint));
        }
        // Check for the endpoint snap condition
        auto distance_from_start = (mouse_pos - start_screen).Length();
        if (distance_from_start < 10) {
            snap_vectors.push_back(SnapVector(start_screen, ReticleType::Endpoint));
        }

        auto distance_from_end = (mouse_pos - end_screen).Length();
        if (distance_from_end < 10) {
            snap_vectors.push_back(SnapVector(end_screen, ReticleType::Endpoint));
        }
    }

    void Visit(CircleObject& object) override
    {
        // Check for the center snap condition
    }

    void Visit(PolylineObject& object) override
    {
        // Check for the midpoint snap condition
        // Check for the endpoint snap condition
        // Check for the intersection snap condition
    }

    std::vector<SnapVector> CollectResults()
    {
        // return a vector of SnapVectors
        auto result = std::move(snap_vectors);
        snap_vectors.clear();
        return result;
    }
};

void Cursor::update_mouse_location(Core::Controller& controller)
{
    Core::Vector2 mouse_position = controller.GetInput().GetMousePosition();
    x = mouse_position.x;
    y = mouse_position.y;
}

void Cursor::snap_cursor_to_grid(Core::Controller& controller, Core::Transform view_transform, float grid_size)
{
    Core::Vector2 mouse_position = controller.GetInput().GetMousePosition();
    Core::Vector2 cursor_world = view_transform.Inverse().Apply(mouse_position);
    float grid_x = std::round(cursor_world.x / grid_size) * grid_size;
    float grid_y = std::round(cursor_world.y / grid_size) * grid_size;
    cursor_world = Core::Vector2(grid_x, grid_y);
    Core::Vector2 snapped_position = view_transform.Apply(cursor_world);
    x = snapped_position.x;
    y = snapped_position.y;
}

void Cursor::collect_snap_vectors(std::vector<SnapVector>& snap_vectors, ObjectRegistry& registry,
    Core::Transform view_transform, float grid_size)
{
    SnapVisitor snap_visitor(Core::Vector2(x, y), view_transform, grid_size);
    registry.VisitObjects(snap_visitor);
    for (auto vector : snap_visitor.CollectResults()) {
        snap_vectors.push_back(std::move(vector));
    }
}

SnapVector Cursor::get_closest_snap_vector(std::vector<SnapVector>& snap_vectors, Core::Vector2 mouse_position)
{
    // Determine the snap vector with the least distance from the mouse position
    SnapVector* closest_snap_vector = &snap_vectors[0];
    int i = 0;
    float min_distance = 0;
    for (auto& vector : snap_vectors) {
        auto distance = (mouse_position - vector.screen_point).Length();
        if (distance < min_distance || i == 0) {
            min_distance = distance;
            closest_snap_vector = &vector;
        }
        i++;
    }
    return *closest_snap_vector;
}

void Cursor::Update(Core::Controller& controller, ObjectRegistry& registry, Core::Transform view_transform,
    float grid_size, RayBank& ray_bank)
{
    // Update x and y to the mouse position, and if grid snapping is enabled, snap to the grid
    update_mouse_location(controller);
    if (is_grid_snapped) {
        snap_cursor_to_grid(controller, view_transform, grid_size);
    }

    // Search for any possible snap vectors
    std::vector<SnapVector> snap_vectors;
    collect_snap_vectors(snap_vectors, registry, view_transform, grid_size);

    std::vector<Core::Vector2> snap_points;
    for (auto vector : ray_bank.GetSnapPoints(registry, view_transform, controller.GetInput().GetMousePosition())) {
        auto snap_vector = SnapVector(vector, ReticleType::Intersection);
        snap_vectors.push_back(snap_vector);
    }

    // If no possible snaps, just draw the gridpoint reticle
    if (snap_vectors.empty()) {
        std::unique_ptr<GridpointReticle> gridpoint_reticle;
        gridpoint_reticle = std::make_unique<GridpointReticle>();
        reticle = std::move(gridpoint_reticle);
        reticle->Draw(controller, Core::Vector2(x, y));
    } else {
        // Otherwise find the closest snap vector to the mouse position and snap to it
        // Then instantiante the corresponding snap reticle and draw it
        auto mouse_position = controller.GetInput().GetMousePosition();
        auto closest_snap_vector = get_closest_snap_vector(snap_vectors, mouse_position);
        x = closest_snap_vector.screen_point.x;
        y = closest_snap_vector.screen_point.y;
        auto reticle = Reticle::FromType(closest_snap_vector.type);
        reticle->Draw(controller, closest_snap_vector.screen_point);
    }

    // Finally draw any raycasts
    // raycast.Draw(controller.GetGraphics(), view_transform);
}
}; // namespace Cad
