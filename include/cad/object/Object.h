#pragma once

#include <core/math/Vector2.h>

#include <memory>
#include <vector>
#include <string>

namespace Cad {

struct ObjectVisitor;

class Object {
    bool is_selected = false;
    public:

    virtual ~Object() = default;
    virtual void Accept(ObjectVisitor& visitor) = 0;
    virtual std::unique_ptr<Object> Clone() const = 0;
    virtual std::string ToString() const = 0;

    bool IsSelected() const;
    void Select();
    void Deselect();
};

struct ObjectPredicate {
    virtual ~ObjectPredicate() = default;
    virtual bool Match(Object& object) = 0;
};

struct LineObject : public Object {
    Core::Vector2 start;
    Core::Vector2 end;

    LineObject(Core::Vector2 start, Core::Vector2 end);
    
    void Accept(ObjectVisitor& visitor) override;
    std::unique_ptr<Object> Clone() const override;
    std::string ToString() const override;
};

struct CircleObject : public Object {
    Core::Vector2 center;
    float radius;

    CircleObject(Core::Vector2 center, float radius);

    void Accept(ObjectVisitor& visitor) override;
    std::unique_ptr<Object> Clone() const override;
    std::string ToString() const override;
};

struct PolylineObject : public Object {
    std::vector<Core::Vector2> points;

    PolylineObject(std::vector<Core::Vector2> points);

    void Accept(ObjectVisitor& visitor) override;
    std::unique_ptr<Object> Clone() const override;
    std::string ToString() const override;
};

}