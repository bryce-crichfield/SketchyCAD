#pragma once

#include <Engine.h>
#include <cad/CadRegistry.h>
#include <cad/CadViewfinder.h>
#include <cad/CadController.h>

namespace Cad {

class Application : public Controller {
    Interactor focus;
    std::unique_ptr<Registry> registry;
    std::unique_ptr<Viewfinder> viewfinder;
    State* state = nullptr;

  public:
    Application(InteractionLock& lock);

    void OnStart(State& state);

    void Update(State& state);
    void OnInput(State state);
    void OnUpdate(State state);
    void OnRender(State state);


    Graphics& GetGraphics() override;
    Input& GetInput() override;
    Chronometer& GetChronometer() override;
    StaticContext& GetStaticContext() override;
    Registry& GetRegistry() override;
    Viewfinder& GetViewfinder() override;
};

} // namespace Cad