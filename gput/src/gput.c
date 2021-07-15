
#include <stdio.h>
#include <stdbool.h>

#include "logger.h"

bool gput_init()
{
   logger_initConsoleLogger(NULL);
   logger_setLevel(LogLevel_TRACE);

   LOG_TRACE("Test trace logging");
   LOG_DEBUG("Test debug logging");
   LOG_INFO("Test info logging");
   LOG_WARN("Test Warn logging");
   LOG_ERROR("Test error logging");
   LOG_FATAL("Test fatal logging");
}
