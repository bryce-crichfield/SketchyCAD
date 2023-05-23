#pragma once

#include <core/Core.h>
#include <core/gui/InteractionLock.h>
#include <core/gui/Interactor.h>

#include <cad/Controller.h>
#include <cad/Dispatcher.h>
#include <cad/Viewfinder.h>
#include <cad/object/ObjectRegistry.h>

#include <iomanip>

namespace Cad {

class InputHandler {
  public:
    virtual ~InputHandler() = default;
    virtual void OnInput(Cad::Controller& controller) = 0;
};

class Application {
    Core::Gui::Interactor focus;
    std::unique_ptr<ObjectRegistry> registry;
    std::unique_ptr<Viewfinder> viewfinder;
    std::unique_ptr<InputHandler> input_handler;
    std::shared_ptr<Dispatcher> dispatcher;

  public:
    Application(Core::Gui::InteractionLock& lock);

    void OnStart(Controller& controller);
    void Update(Controller& controller);
    void OnInput(Controller& controller);
    void OnUpdate(Controller& controller);
    void OnRender(Controller& controller);

    Cad::Controller CreateController(Core::Controller& controller)
    {
        return Cad::Controller(controller, *registry, *viewfinder);
    }

    std::shared_ptr<Dispatcher>& GetDispatcher() { return dispatcher; }
};

} // namespace Cad