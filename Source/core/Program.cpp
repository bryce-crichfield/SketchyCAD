#include <core/Program.h>

#include <core/chrono/Clock.h>
#include <core/State.h>

#include <iostream>

namespace Core {

Program::Program(unsigned view_width, unsigned view_height, unsigned buffer_width, unsigned buffer_height)
{
    // Try to build the viewport which could fail due to system limitations
    try {
        viewport = std::make_unique<Viewport>(view_width, view_height, buffer_width, buffer_height);
    } catch (const std::string& error) {
        std::cerr << "Failed to initialize application" << std::endl;
        std::cerr << error << std::endl;
        exit(1);
    }

    m_output = std::make_unique<BufferedOutput>();
    m_font_manager = std::make_unique<FontManager>();
}

void Program::Execute()
{
    bool is_started = false;
    std::unique_ptr<Clock> clock = std::make_unique<Clock>();
    do {
        clock->Tick();
        viewport->UpdateInput();

        auto input = viewport->GetInput();
        auto graphics = viewport->GetGraphics();

        StateBuilder builder;

        builder.SetChronometer(clock.get());

        builder.SetInput(input.get());
        builder.SetGraphics(graphics.get());
        builder.SetOutput(m_output.get());
        builder.SetFontManager(m_font_manager.get());

        std::unique_ptr<Controller> controller = builder.Build();

        if (!is_started) {
            std::cout << "[INFO] Execution Begun" << std::endl; 
            this->OnStart(*controller);
            is_started = true;
        }
        
        this->OnUpdate(*controller);

        viewport->UpdateFramebuffer();
        viewport->Flush();
        // m_output->Flush();
    } while (viewport->IsOpen());
}
} // namespace Core