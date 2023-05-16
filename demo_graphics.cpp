#include <Cad.h>
#include <Engine.h>

#include <math.h>

struct TextDisplay : public Panel
{
    float padding = 3;

    TextDisplay(unsigned width, unsigned height)
        : Panel(width, height)
    {
    }

    void OnInput(State &state) override
    {
    }

    void OnUpdate(State &state) override
    {
    }

    void OnRender(State &state) override
    {
        float x = GetPosition().x;
        float y = GetPosition().y;
        float w = GetSize().x;
        float h = GetSize().y;

        Font &font = state.static_context.fonts.GetFont("default");
        FontGraphics font_graphics(state.graphics, font);
        Pallette &pallette = state.static_context.pallette;

        state.graphics.FillRect(Color::BLACK, x, y, w, h);
        state.graphics.DrawRect(Color::WHITE, x, y, w, h);

        std::vector<std::string> lines = state.static_context.output.GetLines();
        unsigned font_size = font_graphics.GetDisplayWidth();
        unsigned max_lines = (h - (2 * padding)) / font_size;
        unsigned max_chars = (w - (2 * padding)) / font_size;

        unsigned start_x = x + padding;
        unsigned start_y = (y + h) - (padding + font_size);
        unsigned end_x = (x + w) - (padding + font_size);
        unsigned end_y = y + (padding + font_size);

        unsigned draw_x = start_x;
        unsigned draw_y = start_y;

        unsigned delta_x = font_size;
        unsigned delta_y = -1 * font_size;

        // Draw the lines in reverse order
        for (auto it = lines.rbegin(); it != lines.rend(); it++)
        {
            for (auto character : *it)
            {
                std::string character_string = std::string(1, character);
                font_graphics.DrawString(Color::WHITE, character_string, draw_x, draw_y);
                bool is_overline = draw_x > end_x;
                if (is_overline)
                {
                    draw_x = start_x;
                    draw_y += delta_y;
                }
                else
                {
                    draw_x += delta_x;
                }

                if (draw_y < end_y)
                {
                    return;
                }
            }

            draw_x = start_x;
            draw_y += delta_y;
            draw_y -= 5;
        }
    }
};


struct TextField : public Panel
{
    std::shared_ptr<Cad::Command::Dispatcher> command_dispatcher;
    TextType text_type;
    std::string leader = "> ";

    TextField(std::shared_ptr<Cad::Command::Dispatcher> dispatcher, unsigned width, unsigned height)
        : command_dispatcher(dispatcher), Panel(width, height)
    {
    }

    void OnInput(State &state) override
    {
        auto& output_stream = state.static_context.output;
        text_type.Update(state.input, state.chrono);

        if (state.input.IsPressed(Key::Enter))
        {
            output_stream.Println(leader + text_type.GetText());
            command_dispatcher->Dispatch(text_type.GetText()); // assuming not nullptr because of single constructor
            text_type.Clear();
        }

        if (state.input.IsHeld(Key::LControl) && state.input.IsPressed(Key::Backspace))
        {
            text_type.Clear();
        }
    }

    void OnUpdate(State &state) override
    {
    }

    void OnRender(State &state) override
    {
        float x = GetPosition().x;
        float y = GetPosition().y;
        float width = GetSize().x;
        float height = GetSize().y;

        Font &font = state.static_context.fonts.GetFont("default");
        FontGraphics font_graphics(state.graphics, font);
        Pallette &pallette = state.static_context.pallette;

        // Draw background and border
        state.graphics.FillRect(Color::BLACK, x, y, width, height);
        // Draw border on the left and right, and the bottom
        state.graphics.DrawLine(Color::WHITE, x, y, x, y + height);
        state.graphics.DrawLine(Color::WHITE, x + width, y, x + width, y + height);
        state.graphics.DrawLine(Color::WHITE, x, y + height, x + width, y + height);

        // Calculate the max text length, drop the first n characters if it is too long
        unsigned font_size = font_graphics.GetDisplayWidth();
        unsigned max_text_length = (width - font_size) / font_size;
        std::string text = leader + text_type.GetText();
        if (text.length() > max_text_length)
        {
            text = text.substr(text.length() - max_text_length, max_text_length);
        }
        // Re-add the leader character
        text = leader + text.substr(leader.length(), text.length() - leader.length());
        float text_y = y + (height - font_size) / 2;
        // Draw the text at the center of the text field
        font_graphics.DrawString(Color::GREEN, text, x, text_y);
    }
};

struct CommandWindow : public Window
{
    std::shared_ptr<TextField> text_field;
    std::shared_ptr<TextDisplay> text_display;
    std::shared_ptr<Cad::Command::Dispatcher> command_dispatcher;

    CommandWindow(InteractionLock &lock, std::shared_ptr<Cad::Command::Dispatcher> dispatcher)
        : Window(lock, Shortcuts::ctrl_n1, 30, 30, 100, 100), command_dispatcher(dispatcher)
    {
        text_field = std::make_shared<TextField>(dispatcher, 100, 15);
        text_display = std::make_shared<TextDisplay>(100, 80);
        GetRoot().AddChild(text_display);
        GetRoot().AddChild(text_field);
    }

    void OnInput(State state) override
    {
    }

    void OnUpdate(State state) override
    {
        text_field->SetSize(GetSize().x, 15);
        text_display->SetSize(GetSize().x, GetSize().y - 15);
    }

    void OnRender(State state) override
    {
    }
};

#define VIEW_SIZE_WIDTH 700
#define VIEW_SIZE_HEIGHT 700

struct CadProgram : Program
{
    InteractionLock lock;
    std::shared_ptr<Cad::Command::Dispatcher> command_dispatcher = std::make_shared<Cad::Command::Dispatcher>();
    Cad::Controller cad = Cad::Controller(lock);

    CommandWindow t2 = CommandWindow(lock, command_dispatcher);

    CadProgram()
    {
        t2.SetPosition(0, VIEW_SIZE_HEIGHT - 200);
        t2.SetSize(VIEW_SIZE_WIDTH / 2, 200);
        t2.SetTitle("Command Window");
    }

    void OnStart(State &state) override
    {
        state.static_context.fonts.LoadFont("default", "assets/font/dogica.bin", 8);

        command_dispatcher->Register(std::make_unique<Cad::Command::SnapCommand::Signature>());
        command_dispatcher->Register(std::make_unique<Cad::Command::ClearCommand::Signature>());
        command_dispatcher->Register(std::make_unique<Cad::Command::CreateCircle::By2Point>());
        command_dispatcher->Register(std::make_unique<Cad::Command::CreateCircle::ByRadius>());
        command_dispatcher->Register(std::make_unique<Cad::Command::CreateLine::By2Point>());
        command_dispatcher->Register(std::make_unique<Cad::Command::CreateLine::ByStartAngleLength>());

        command_dispatcher->Register(std::make_unique<Cad::Command::ToggleSnapCommand::Signature>());
        command_dispatcher->Register(std::make_unique<Cad::Command::TranslateCommand::Signature>());
        command_dispatcher->Register(std::make_unique<Cad::Command::PanCommand::Signature>());
        command_dispatcher->Register(std::make_unique<Cad::Command::ZoomCommand::Signature>());
        command_dispatcher->Register(std::make_unique<Cad::Command::PasteCommand::Signature>());

        cad.OnStart(state);

        // TODO: Figure out what to do with the window manager... maybe remove it?
        // WindowManager &window_manager = context.window_manager;
        // window_manager.RegisterWindow(&t2);
    }

    void OnUpdate(State state) override
    {
        state.graphics.Clear(Color::BLACK);
        command_dispatcher->Execute(cad);
        cad.Update(state);
        t2.Update(state);
    }
};

int main(void)
{
    Engine engine(1300, 1300, VIEW_SIZE_WIDTH, VIEW_SIZE_HEIGHT);
    CadProgram program;
    engine.Launch(program);
}