#include <iostream>

#include "App.hpp"
#include "Dialogs.hpp"

int main()
{
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