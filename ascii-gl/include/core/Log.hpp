#pragma once

#include <memory>
#include <string_view>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/bundled/ostream.h>

class Log
{
public:

  static void Init();

  inline static std::shared_ptr<spdlog::logger> &GetLogger() { return s_logger; }

private:
  static std::shared_ptr<spdlog::logger> s_logger;
};

// Custom formater for spdlog so that anything with either
// - a << operator on std::ostream
// - a ToString() member function
// can be logged seamlessly

// Concept for ostream injectable data types
template<typename T>
concept IsPrintable = std::is_compound_v<T> && !std::is_pointer_v<T> && requires(std::ostream & os, const T & data) {
  os << data;
};

// Concept for data types that have a public ToString() member function
template<typename T>
concept IsStringifiable = std::is_compound_v<T> && requires(const T & data) {
  { data.ToString() } -> std::convertible_to<std::string>;
};

// declare the specification for any type that match with either one of the above concepts
template <typename T> requires IsPrintable<T> || IsStringifiable<T>
struct fmt::formatter<T> : ostream_formatter {};

// delcare the function used to format stringifiable data types
template<typename T> requires IsStringifiable<T>
inline std::ostream &operator<<(std::ostream &os, const T &data)
{
  return os << data.ToString();
}

#define SET_LOG_LEVEL(log_level) ::Log::GetLogger()->set_level(spdlog::level:: log_level)
#define LOG_DEBUG(...) ::Log::GetLogger()->debug(__VA_ARGS__)
#define LOG_TRACE(...) ::Log::GetLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...)  ::Log::GetLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)  ::Log::GetLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...) ::Log::GetLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::Log::GetLogger()->critical(__VA_ARGS__)
