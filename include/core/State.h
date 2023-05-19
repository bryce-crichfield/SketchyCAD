#pragma once

#include <core/Controller.h>

#include <stdexcept>

namespace Core {

class StateBuilder;

struct State : public Controller {
    friend class StateBuilder;
    Input* input;
    Output* output;
    Graphics* graphics;
    Chronometer* chrono;
    FontManager* fonts;
    Gui::StyleManager* style_manager;

    State() = default;

  public:
  
    Chronometer& GetChronometer() override { return *chrono; }
    Input& GetInput() override { return *input; }
    Output& GetOutput() override { return *output; }
    Graphics& GetGraphics() override { return *graphics; }
    FontManager& GetFontManager() override { return *fonts; }
    Gui::StyleManager& GetStyleManager() override { return *style_manager; }
};

struct StateBuilder {
    std::unique_ptr<State> state;

    StateBuilder() {
        state = std::make_unique<State>();
    }

    StateBuilder& SetChronometer(Chronometer* chrono)
    {
        state->chrono = chrono;
        return *this;
    }

    StateBuilder& SetInput(Input* input)
    {
        state->input = input;
        return *this;
    }

    StateBuilder& SetOutput(Output* output)
    {
        state->output = output;
        return *this;
    }

    StateBuilder& SetGraphics(Graphics* graphics)
    {
        state->graphics = graphics;
        return *this;
    }

    StateBuilder& SetFontManager(FontManager* fonts)
    {
        state->fonts = fonts;
        return *this;
    }

    StateBuilder& SetStyleManager(Gui::StyleManager* style_manager)
    {
        state->style_manager = style_manager;
        return *this;
    }

    std::unique_ptr<State> Build()
    {
        // ensure that all fields are set
        if (!state->chrono || !state->input || !state->output || !state->graphics || !state->fonts || !state->style_manager) {
            throw std::runtime_error("StateBuilder: not all fields are set");
        }
        return std::move(state);
    }
};

} // namespace Core