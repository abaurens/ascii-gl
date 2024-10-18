#include <iostream>

#include "App.hpp"
#include "Dialogs.hpp"

#include "core/Log.hpp"

int main()
{
  Log::Init();
  
  
  SET_LOG_LEVEL(warn);
  
  std::cin.get();
  
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