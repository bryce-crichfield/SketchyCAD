#pragma once

#include <cad/object/Object.h>

namespace Cad {
struct ObjectBuilder {
    virtual std::unique_ptr<Object> Build() = 0;
};

struct LineObjectBuilder : ObjectBuilder {
    Core::Vector2 start;
    Core::Vector2 end;

    LineObjectBuilder(Core::Vector2 start, Core::Vector2 end) : start(start), end(end) {}

    std::unique_ptr<Object> Build() override {
        return std::make_unique<LineObject>(start, end);
    }
};

struct CircleObjectBuilder : ObjectBuilder {
    Core::Vector2 center;
    float radius;

    CircleObjectBuilder(Core::Vector2 center, float radius) : center(center), radius(radius) {}

    std::unique_ptr<Object> Build() override {
        return std::make_unique<CircleObject>(center, radius);
    }
};

struct PolylineObjectBuilder : ObjectBuilder {
    std::vector<Core::Vector2> points;

    PolylineObjectBuilder(std::vector<Core::Vector2> points) : points(points) {}

    std::unique_ptr<Object> Build() override {
        return std::make_unique<PolylineObject>(points);
    }
};
}