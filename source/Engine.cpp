#include <Engine.h>

void Engine::updateDebugStats(double delta_time)
{
    accumulated_delta_time += delta_time;
    accumlated_updates_count++;
    if (accumlated_updates_count > 60)
    {
        double average_delta_time = accumulated_delta_time / accumlated_updates_count;
        viewport->SetTitle("FPS: " + std::to_string(1.0 / average_delta_time));
        accumulated_delta_time = 0;
        accumlated_updates_count = 0;
    }
}

Engine::Engine(unsigned view_width, unsigned view_height, unsigned buffer_width, unsigned buffer_height)
{
    try
    {
        viewport = std::make_unique<Viewport>(view_width, view_height, buffer_width, buffer_height);
        m_fonts = std::make_unique<FontManager>();
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

void Engine::Launch()
{
    StaticContext context(*m_fonts);

    for (auto &extension : m_extensions)
    {
        extension->OnStart(context);
    }

    Clock clock;
    while (viewport->IsOpen())
    {
        clock.Tick();
        updateDebugStats(clock.GetDelta());

        Input input = viewport->UpdateInput();
        Graphics graphics = viewport->GetGraphics();
        graphics.Clear(Color::BLACK);

        State state(clock, input, graphics, context);
        for (auto &extension : m_extensions)
        {
            extension->OnUpdate(state);
        }

        viewport->UpdateFramebuffer();
        viewport->Flush();
    }
}

void Engine::AddExtension(std::shared_ptr<Extension> extension)
{
    m_extensions.push_back(extension);
}