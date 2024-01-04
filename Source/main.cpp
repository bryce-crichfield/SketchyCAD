#include <cad/Cad.h>
#include <core/Core.h>
// #include <cad/Cad.h>
// #include <app/Commands.h>
#include <cad/gui/Gui.h>

#define VIEW_SIZE_WIDTH 700
#define VIEW_SIZE_HEIGHT 700

struct HelpCommand : public Cad::Command {
    std::shared_ptr<Cad::Dispatcher> dispatcher;

    HelpCommand(std::shared_ptr<Cad::Dispatcher> dispatcher) : dispatcher(dispatcher) {}

    void Forward(Cad::Controller& cad) override {
        // Get all registered commands
        auto commands = dispatcher->GetCommands();
        auto& output = cad.GetOutput();
        for (auto& command : commands) {
            output.Writeln(command->GetName());
        }
    }

    void Backward(Cad::Controller& cad) override { std::cout << "undo" << std::endl; }

    struct Signature : Cad::Syntax::ArgSignature {
        std::shared_ptr<Cad::Dispatcher> dispatcher;

        Signature(std::shared_ptr<Cad::Dispatcher> dispatcher) : dispatcher(dispatcher), ArgSignature("help", {}) {}

        std::unique_ptr<Cad::Command> Create(std::vector<Cad::Syntax::Arg> args) override {
            return std::make_unique<HelpCommand>(dispatcher);
        }
    };
};

struct CadProgram : public Core::Program {
    Cad::Application cad;

    CadProgram() : cad(), Program(1300, 1300, VIEW_SIZE_WIDTH, VIEW_SIZE_HEIGHT) {}

    void OnStart(Core::Controller& controller) override {
        controller.GetFontManager().LoadFont("default", "../Assets/Font/dogica.bin", 8);
        Cad::Controller cad_controller = cad.CreateController(controller);

        cad.OnStart(cad_controller);

        cad.GetDispatcher()->Register(std::make_unique<Cad::ZoomCommand::Signature>());
        cad.GetDispatcher()->Register(std::make_unique<Cad::PanCommand::Signature>());
        cad.GetDispatcher()->Register(std::make_unique<Cad::ZeroCommand::Signature>());
        cad.GetDispatcher()->Register(std::make_unique<Cad::CreateCircle::Signature>());
        cad.GetDispatcher()->Register(std::make_unique<HelpCommand::Signature>(cad.GetDispatcher()));
    }

    void OnUpdate(Core::Controller& controller) override {
        controller.GetGraphics().Clear(Core::Color::BLACK);
        Cad::Controller cad_controller = cad.CreateController(controller);
        cad.Update(cad_controller);
    }

    void OnShutdown(Core::Controller& controller) override {}
};

int main(void) {
    std::unique_ptr<Core::Program> program = std::make_unique<CadProgram>();
    program->Execute();
}