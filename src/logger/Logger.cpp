#include "logger/logger.hpp"

namespace memo::logger {


void Init(const std::string& logFilePath)
{
    el::Configurations config;
    config.setToDefault();
    config.setGlobally(el::ConfigurationType::Format, "[%datetime{%d/%M/%Y %H:%m:%s}] %levshort %fbase:%line %msg");
    config.setGlobally(el::ConfigurationType::Filename, logFilePath);
    el::Loggers::reconfigureLogger("default", config);
    el::Loggers::addFlag(el::LoggingFlag::ImmediateFlush);
    el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
}

} // namespace memo::logger
