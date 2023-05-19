#pragma once

#include <core/Core.h>
#include <core/gui/Interactor.h>
#include <core/gui/InteractionLock.h>


#include <cad/object/ObjectRegistry.h>
#include <cad/Viewfinder.h>
#include <cad/Controller.h>


namespace Cad {

class Application  {
    Core::Gui::Interactor focus;
    std::unique_ptr<ObjectRegistry> registry;
    std::unique_ptr<Viewfinder> viewfinder;

  public:
    Application(Core::Gui::InteractionLock& lock);

    void OnStart(Controller& controller);
    void Update(Controller& controller);
    void OnInput(Controller& controller);
    void OnUpdate(Controller& controller);
    void OnRender(Controller& controller);

    Cad::Controller CreateController(Core::Controller& controller) {
        return Cad::Controller(controller, *registry, *viewfinder);
    }
};

} // namespace Cad