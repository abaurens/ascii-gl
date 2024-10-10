#include "Dialogs.hpp"
#include "core/compile_random.hpp"

#include <Windows.h>
#include <sstream>

HWND GetConsoleHandle()
{
  static HWND console_handle = nullptr;

  if (!console_handle)
  {
    WCHAR window_title_save[2048];

    GetConsoleTitle(window_title_save, 2048);

    std::stringstream ss;
    ss << "ConsoleLookup#" << COMPILE_RANDOM();
    SetConsoleTitleA(ss.str().c_str());
    Sleep(40);

    console_handle = FindWindowA(nullptr, ss.str().c_str());

    SetConsoleTitle(window_title_save);
  }

  return console_handle;
}

namespace dial
{
  void Message(const std::string_view title, const std::string_view message)
  {
    MessageBoxA(GetConsoleHandle(), message.data(), title.data(), MB_OK);
  }

  void Warning(const std::string_view title, const std::string_view message)
  {
    MessageBoxA(GetConsoleHandle(), message.data(), title.data(), MB_OK | MB_ICONWARNING);
  }

  void Error(const std::string_view title, const std::string_view message)
  {
    MessageBoxA(GetConsoleHandle(), message.data(), title.data(), MB_OK | MB_ICONHAND);
  }

  void Critical(const std::string_view title, const std::string_view message)
  {
    MessageBoxA(GetConsoleHandle(), message.data(), title.data(), MB_OK | MB_ICONHAND);
    exit(1);
  }
}