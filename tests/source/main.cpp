#include <iostream>

#include "App.hpp"
#include "Dialogs.hpp"

#include "core/Log.hpp"

int main()
{
  Log::Init();

  ::Log::GetLogger()->set_level(spdlog::level::warn);

  try
  {
    App app;
    app.Run();
  }
  catch (const std::exception &exception)
  {
    dial::Critical("Unhandled exception", exception.what());
  }
  return 0;
}