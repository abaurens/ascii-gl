#pragma once

#include <string>
#include <string_view>

namespace dial
{
  void Message(const std::string_view title, const std::string_view message);
  void Warning(const std::string_view title, const std::string_view message);
  void Error(const std::string_view title, const std::string_view message);
  void Critical(const std::string_view title, const std::string_view message);

  inline void Message(const std::string_view message)  { return Message("Info",            message); }
  inline void Warning(const std::string_view message)  { return Warning("Warning",         message); }
  inline void Error(const std::string_view message)    { return Error("Error",             message); }
  inline void Critical(const std::string_view message) { return Critical("Critical error", message); }
}
