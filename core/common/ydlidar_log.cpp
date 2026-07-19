#include "ydlidar_help.h"

#include <cstdio>
#include <ctime>
#include <mutex>

namespace ydlidar {
namespace core {
namespace common {
namespace {

std::mutex logMutex;
LogCallback logCallback = NULL;
void *logUserData = NULL;

const char *levelName(LogLevel level)
{
  switch (level) {
    case LogLevelDebug: return "debug";
    case LogLevelInfo: return "info";
    case LogLevelWarn: return "warn";
    case LogLevelError: return "error";
  }
  return "info";
}

}  // namespace

void setLogCallback(LogCallback callback, void *userData)
{
  std::lock_guard<std::mutex> lock(logMutex);
  logCallback = callback;
  logUserData = userData;
}

void dispatchLog(LogLevel level, const char *message)
{
  std::lock_guard<std::mutex> lock(logMutex);
  if (logCallback) {
    logCallback(level, message, logUserData);
    return;
  }

  const std::time_t now = std::time(NULL);
  std::tm localTime;
#ifdef _WIN32
  localtime_s(&localTime, &now);
#else
  localtime_r(&now, &localTime);
#endif
  std::fprintf(stdout, "[%04d-%02d-%02d %02d:%02d:%02d][%s] %s\n",
               1900 + localTime.tm_year, 1 + localTime.tm_mon,
               localTime.tm_mday, localTime.tm_hour, localTime.tm_min,
               localTime.tm_sec, levelName(level), message ? message : "");
  std::fflush(stdout);
}

}  // namespace common
}  // namespace core
}  // namespace ydlidar
