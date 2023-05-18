#include <Engine.h>

#include <cad/CadObject.h>
#include <cad/CadRegistry.h>
#include <cad/CadApplication.h>
#include <cad/CadViewfinder.h>
#include <cad/CadController.h>
#include <cad/CadCommand.h>
#include <cad/CadGui.h>

#define VIEW_SIZE_WIDTH 700
#define VIEW_SIZE_HEIGHT 700

struct EchoCommand : public Cad::Command {
    std::string message;

    EchoCommand(std::string message) : message(message) { }

    void Forward(Cad::Controller& cad) override { cad.GetStaticContext().output.PushLine(message, Color::WHITE); }

    void Backward(Cad::Controller &cad) override {
        std::cout << "undo" << std::endl;
    }

    struct Signature : public Cad::Syntax::ArgSignature {
        Signature() : ArgSignature("echo", {Cad::Syntax::Arg::Type::STRING}) {}

        std::unique_ptr<Cad::Command> Create(std::vector<Cad::Syntax::Arg> args) override {
            std::string message = args[0].AsString();
            return std::make_unique<EchoCommand>(message);;
        }
    };
};

struct CadProgram : public Program {
    InteractionLock lock;
    Cad::Application app = Cad::Application(lock);
    std::shared_ptr<Cad::Dispatcher> dispatcher = std::make_shared<Cad::Dispatcher>();
    Cad::Gui::CommandWindow console = Cad::Gui::CommandWindow(lock, dispatcher);

    CadProgram() = default;

    void OnStart(State &state) override {
        state.static_context.fonts.LoadFont("default", "../assets/font/dogica.bin", 8);

        app.OnStart(state);

        console.SetPosition(0, VIEW_SIZE_HEIGHT - 200);
        console.SetSize(VIEW_SIZE_WIDTH / 2, 200);
        console.SetTitle("Command Window");

        dispatcher->Register(std::make_unique<EchoCommand::Signature>());
    }

    void OnUpdate(State state) override {
        state.graphics.Clear(Color::BLACK);
        app.Update(state);
        console.Update(state);
        dispatcher->Execute(app);
    }
};

int main(void) {
    Engine engine(1300, 1300, VIEW_SIZE_WIDTH, VIEW_SIZE_HEIGHT);
    CadProgram program;
    engine.Launch(program);
}
