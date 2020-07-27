#pragma once
#include "logger/easylogging++.h"

#include <string>

namespace memo {
namespace logger {

void Init(const std::string& iLogFilePath);

} // namespace logger
} // namespace memo

#define LOG_TRC(ARG) LOG(TRACE)   << ARG;
#define LOG_DBG(ARG) LOG(DEBUG)   << ARG;
#define LOG_INF(ARG) LOG(INFO)    << ARG;
#define LOG_WRN(ARG) LOG(WARNING) << ARG;
#define LOG_ERR(ARG) LOG(ERROR)   << ARG;
#define LOG_FTL(ARG) LOG(FATAL)   << ARG;
