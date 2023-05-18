#pragma once

#include <Engine.h>
#include <cad/CadRegistry.h>
#include <cad/CadViewfinder.h>

namespace Cad {
struct Controller {
    virtual Graphics& GetGraphics() = 0;
    virtual Input& GetInput() = 0;
    virtual Chronometer& GetChronometer() = 0;
    virtual StaticContext& GetStaticContext() = 0;
    virtual Cad::Registry& GetRegistry() = 0;
    virtual Cad::Viewfinder& GetViewfinder() = 0;
};
}