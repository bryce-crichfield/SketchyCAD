#include <core/Core.h>
#include <cad/Cad.h>
// #include <cad/Cad.h>
// #include <app/Commands.h>

#define VIEW_SIZE_WIDTH 700
#define VIEW_SIZE_HEIGHT 700

struct CadProgram : public Core::Program {
    Core::Gui::InteractionLock lock;
    Cad::Application cad_application;
    std::shared_ptr<Cad::Dispatcher> command_dispatcher = std::make_shared<Cad::Dispatcher>();
    Cad::Gui::CommandWindow command_window = Cad::Gui::CommandWindow(lock, command_dispatcher);

    CadProgram() : cad_application(lock), Program(1300, 1300, VIEW_SIZE_WIDTH, VIEW_SIZE_HEIGHT) {}

    void OnStart(Core::Controller& controller) override {
        controller.GetFontManager().LoadFont("default", "../assets/font/dogica.bin", 8);
        Cad::Controller cad_controller = cad_application.CreateController(controller);
        cad_application.OnStart(cad_controller);

        command_window.SetPosition(0, VIEW_SIZE_HEIGHT);
        command_window.SetSize(300, 400);

        command_dispatcher->Register(std::make_unique<Cad::ZoomCommand::Signature>());
        command_dispatcher->Register(std::make_unique<Cad::PanCommand::Signature>());
        command_dispatcher->Register(std::make_unique<Cad::ZeroCommand::Signature>());
    }

    void OnUpdate(Core::Controller& controller) override
    {
        controller.GetGraphics().Clear(Core::Color::BLACK);

        Cad::Controller cad_controller = cad_application.CreateController(controller);
        cad_application.Update(cad_controller);
        command_window.Update(cad_controller);
        command_dispatcher->Execute(cad_controller);
    }

    void OnShutdown(Core::Controller& controller) override {}
};

int main(void)
{
    std::unique_ptr<Core::Program> program = std::make_unique<CadProgram>();
    program->Execute();
}