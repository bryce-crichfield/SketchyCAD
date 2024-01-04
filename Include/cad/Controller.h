#pragma once

#include <core/Controller.h>
#include <cad/object/ObjectRegistry.h>
#include <cad/Viewfinder.h>

namespace Cad {

struct Controller : public Core::Controller {
    Core::Controller& controller;
    ObjectRegistry& registry;
    Viewfinder& viewfinder;
    RayBank& ray_bank;

    Controller(Core::Controller& controller, ObjectRegistry& registry, Viewfinder& viewfinder, RayBank& ray_bank) : 
        controller(controller), registry(registry), viewfinder(viewfinder), ray_bank(ray_bank) {}

    Core::Graphics& GetGraphics() override {
        return controller.GetGraphics();
    }



    Core::Input& GetInput() override {
        return controller.GetInput();
    }

    Core::Output& GetOutput() override {
        return controller.GetOutput();
    }

    Core::Chronometer& GetChronometer() override {
        return controller.GetChronometer();
    }

    Core::FontManager& GetFontManager() override {
        return controller.GetFontManager();
    }

    virtual Cad::ObjectRegistry& GetRegistry() { 
        return registry;
    }

    virtual Cad::RayBank& GetRayBank() {
        return ray_bank;
    }

    virtual Cad::Viewfinder& GetViewfinder() {
        return viewfinder;
    }
};
}