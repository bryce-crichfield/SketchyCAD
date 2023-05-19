#pragma once

#include <core/Controller.h>
#include <core/Viewport.h>

namespace Core {

class Program {
    std::unique_ptr<Viewport> viewport;
    std::unique_ptr<Output> m_output;
    std::unique_ptr<FontManager> m_font_manager;
    std::unique_ptr<Gui::StyleManager> m_style_manager;

public:
  Program(unsigned view_width, unsigned view_height, unsigned buffer_width, unsigned buffer_height);
  virtual ~Program() = default;

  void Execute();

  virtual void OnStart(Controller& controller) = 0;
  virtual void OnUpdate(Controller& controller) = 0;
  virtual void OnShutdown(Controller& controller) = 0;
};
} // namespace Core
