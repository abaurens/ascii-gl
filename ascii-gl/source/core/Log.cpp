#include "core/Log.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

enum LogSinks
{
  //Terminal,
  LogFile,

  Count
};

std::shared_ptr<spdlog::logger> Log::s_logger;

void Log::Init()
{
  // Setup Sinks
  std::array<spdlog::sink_ptr, Count> logSinks;
  logSinks[LogFile] = std::make_shared<spdlog::sinks::basic_file_sink_mt>("asciigl.log", true);
  //logSinks[Terminal] = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

  logSinks[LogFile]->set_pattern("[%T] [%l] %n: %v");
  //logSinks[Terminal]->set_pattern("%^[%T] %n: %v%$");

  // Setup Loggers
  s_logger = std::make_shared<spdlog::logger>("LOG", begin(logSinks), end(logSinks));
  spdlog::register_logger(s_logger);
  s_logger->set_level(spdlog::level::trace);
  s_logger->flush_on(spdlog::level::trace);

  ::Log::GetLogger()->info("Initialized Log!");
}