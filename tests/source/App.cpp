#include "App.hpp"

#include <iostream>
#include <iomanip>

App::App()
{
  m_terminal = Terminal::Create();

  m_terminal->SetUserPointer(this);

  //m_terminal->SetMouseMoveCallback([](Terminal &term, float x, float y) {
  //  term.SetCursorPos((unsigned int)x, (unsigned int)y);
  //});

  m_terminal->SetResizeCallback([](Terminal &term, size_t width, size_t height) {
    std::cout << "\033[H"
      << "Width  : " << std::setw(4) << width << '\n'
      << "Height : " << std::setw(4) << height << "\n\033[H";
    std::cout.flush();
  });

  m_terminal->SetKeyPressedCallback([](Terminal &term, uint32_t key, uint32_t scancode) {
    /// TODO: Implement a proper key/button mapping

    // check if the escape key is pressed
    if (key == 27)
    {
      static_cast<App *>(term.GetUserPointer())->Stop();
    }
  });
}

App::App(int ac, char **av) : App()
{

}

void App::Run()
{
  m_running = true;

  while (m_running)
  {
    m_terminal->PollEvents();
  }
}

void App::Stop() { m_running = false; }