#pragma once

#include <Cad.h>

namespace Cad
{
void Controller::OnStart(State& state) { this->state = &state; }

void Controller::OnInput(State &state)
{
    // Switch mode
    if (state.input.IsPressed(Key::L))
    {
        current_mode = std::make_unique<Mode::Line>();
    }
    else if (state.input.IsPressed(Key::C))
    {
        current_mode = std::make_unique<Mode::Circle>();
    }
    else if (state.input.IsPressed(Key::S))
    {
        current_mode = std::make_unique<Mode::Select>();
    }

    // Trigger undo
    if (state.input.IsPressed(Key::Z) && state.input.IsHeld(Key::LControl))
    {
        Undo();
    }

    cursor.OnInput(*this);
}

void Controller::OnUpdate(State &state)
{
    // TODO: Add object snapping back
    camera_controller.Update(*this);
    cursor.OnUpdate(*this);

    if (current_mode != nullptr)
    {
        current_mode->Update(*this);
    }
}

void Controller::OnRender(State &state)
{
    world.Update(*this);

    if (static_interactor.HasLock())
    {
        cursor.OnRender(*this);
    }
}

void Controller::Update(State &state)
{
    this->state = &state;
    if (static_interactor.TryLock())
    {
        OnInput(state);
        OnUpdate(state);
    }

    OnRender(state);
}

void Controller::Command(std::unique_ptr<ICommand> command)
{
    command->Forward(*this);
    undo_stack.push(std::move(command));
}

void Controller::Undo()
{
    if (undo_stack.size() > 0)
    {
        std::unique_ptr<ICommand> command = std::move(undo_stack.top());
        undo_stack.pop();
        command->Backward(*this);
    }
}

Graphics &Controller::GetGraphics()
{
    return state->graphics;
}

Input &Controller::GetInput()
{
    return state->input;
}

Chronometer &Controller::GetChrono()
{
    return state->chrono;
}

World &Controller::GetWorld()
{
    return world;
}

Camera& Controller::GetCamera() { return camera_controller.GetCamera(); }
} // namespace Cad
