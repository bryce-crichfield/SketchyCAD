#pragma once

#include <cad/object/Object.h>

namespace Cad {

struct ObjectVisitor {
    ~ObjectVisitor() = default;
    virtual void Visit(LineObject& line) = 0;
    virtual void Visit(CircleObject& circle) = 0;
    virtual void Visit(PolylineObject& polyline) = 0;
};

}