#pragma once

#include <Engine.h>

namespace Cad {

struct ObjectVisitor;

class Selection {
    bool m_is_selected = false;
public:
    Selection() = default;
    bool IsSelected() const { return m_is_selected; }
    void Select() { m_is_selected = true; }
    void Deselect() { m_is_selected = false; }
};

struct Object {
    Selection selection;

    virtual ~Object() = default;
    virtual void Accept(ObjectVisitor& visitor) = 0;
    virtual std::unique_ptr<Object> Clone() const = 0;
    virtual std::string ToString() const = 0;
};

struct ObjectPredicate {
    virtual ~ObjectPredicate() = default;
    virtual bool Match(Object& object) = 0;
};

struct LineObject : public Object {
    Vector2 start;
    Vector2 end;

    LineObject(Vector2 start, Vector2 end);
    
    void Accept(ObjectVisitor& visitor) override;
    std::unique_ptr<Object> Clone() const override;
    std::string ToString() const override;
};

struct CircleObject : public Object {
    Vector2 center;
    float radius;

    CircleObject(Vector2 center, float radius);

    void Accept(ObjectVisitor& visitor) override;
    std::unique_ptr<Object> Clone() const override;
    std::string ToString() const override;
};

struct PolylineObject : public Object {
    std::vector<Vector2> points;

    PolylineObject(std::vector<Vector2> points);

    void Accept(ObjectVisitor& visitor) override;
    std::unique_ptr<Object> Clone() const override;
    std::string ToString() const override;
};

struct ObjectVisitor {
    ~ObjectVisitor() = default;
    virtual void Visit(LineObject& line) = 0;
    virtual void Visit(CircleObject& circle) = 0;
    virtual void Visit(PolylineObject& polyline) = 0;
};

struct ObjectBuilder {
    virtual std::unique_ptr<Object> Build() = 0;
};

struct LineObjectBuilder : ObjectBuilder {
    Vector2 start;
    Vector2 end;

    LineObjectBuilder(Vector2 start, Vector2 end) : start(start), end(end) {}

    std::unique_ptr<Object> Build() override {
        return std::make_unique<LineObject>(start, end);
    }
};

struct CircleObjectBuilder : ObjectBuilder {
    Vector2 center;
    float radius;

    CircleObjectBuilder(Vector2 center, float radius) : center(center), radius(radius) {}

    std::unique_ptr<Object> Build() override {
        return std::make_unique<CircleObject>(center, radius);
    }
};

struct PolylineObjectBuilder : ObjectBuilder {
    std::vector<Vector2> points;

    PolylineObjectBuilder(std::vector<Vector2> points) : points(points) {}

    std::unique_ptr<Object> Build() override {
        return std::make_unique<PolylineObject>(points);
    }
};

} // namespace Cad::Core