#include <Engine.h>

Engine::Engine(unsigned view_width, unsigned view_height, unsigned buffer_width, unsigned buffer_height)
{
    try
    {
        viewport = std::make_unique<Viewport>(view_width, view_height, buffer_width, buffer_height);
    }
    catch (const std::string &error)
    {
        std::cerr << "Failed to initialize application" << std::endl;
        std::cerr << error << std::endl;
        exit(1);
    }
}

Engine::~Engine()
{
    // TODO: Shutdown all extensions
}

void Engine::Launch(Program &program)
{
    bool is_started = false;
    Clock clock;
    do
    {
        clock.Tick();
        StaticContext context(m_font_manager, m_output_stream, m_window_manager, m_pallette);
        Input input = viewport->UpdateInput();
        Graphics graphics = viewport->GetGraphics();
        State state(clock, input, graphics, context);
        if (!is_started) {
            program.OnStart(state);
            is_started = true;
        } 
        program.OnUpdate(state);
        m_window_manager.Update(state);
        viewport->UpdateFramebuffer();
        viewport->Flush();
        // m_output_stream.Flush();
    } while (viewport->IsOpen());
}