#pragma once

#include "Terminal.hpp"

#include "graphics/Context.hpp"

class App
{
public:
  App();
  App(int ac, char **av);

  void Run();
  void Stop();
  bool Running() const { return m_running; }

private:

  bool m_running = false;
  Ref<Terminal> m_terminal;

  Context m_glContext;
};
