#pragma once

#include "logger.h"

#include <cstdlib>
#include <format>

namespace Assert {

inline void fail(const char *condition, const char *message, const char *file,
                 int line) {

  Logger::log(LogLevel::ERR, std::format("ASSERT FAILED\n"
                                         "Condition : {}\n"
                                         "Message   : {}\n"
                                         "File      : {}\n"
                                         "Line      : {}",
                                         condition, message, file, line));

  std::abort();
}

} // namespace Assert

#define RT_ASSERT(condition, message)                                          \
  do {                                                                         \
    if (Config::ENABLE_ASSERTS && !(condition)) {                              \
      Assert::fail(#condition, message, __FILE__, __LINE__);                   \
    }                                                                          \
  } while (0)
