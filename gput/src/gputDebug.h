/**
 * MIT License
 *
 * Copyright (c) 2021 Mehdi Nasef
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#pragma once

#include <assert.h>
#include <stdbool.h>

#include <libgen.h>

#include "glad/glad.h"

#include "logger.h"

#define GPUT_LOG_LEVEL_TRACE  LogLevel_TRACE
#define GPUT_LOG_LEVEL_DEBUG  LogLevel_DEBUG
#define GPUT_LOG_LEVEL_INFO   LogLevel_INFO
#define GPUT_LOG_LEVEL_WARN   LogLevel_WARN
#define GPUT_LOG_LEVEL_ERROR  LogLevel_ERROR
#define GPUT_LOG_LEVEL_FATAL  LogLevel_FATAL
#define GPUT_LOG_LEVEL_OFF    LogLevel_FATAL + 1

#define GPUT_CONSOLE_LOG   0
#define GPUT_FILE_LOG      1
#define GPUT_MULTI_LOG     2


#ifndef GPUT_ACTIVE_LOG_LEVEL
   #define GPUT_ACTIVE_LOG_LEVEL GPUT_LOG_LEVEL_INFO
#endif

#ifndef GPUT_LOG_MODE
   #define GPUT_LOG_MODE GPUT_CONSOLE_LOG
#endif

#if GPUT_ACTIVE_LOG_LEVEL != GPUT_LOG_LEVEL_OFF

      #ifndef GPUT_LOG_FILE_NAME
         #define GPUT_LOG_FILE_NAME "gput.log"
      #endif

      #ifndef GPUT_MAX_LOG_FILE_SIZE
         #define GPUT_MAX_LOG_FILE_SIZE 8 * 1024 // 8KiB
      #endif

      #ifndef GPUT_MAX_LOG_BACKUP_FILES
         #define GPUT_MAX_LOG_BACKUP_FILES 0
      #endif

   #if GPUT_LOG_MODE == GPUT_CONSOLE_LOG

      #define GPUT_LOG_INIT() logger_initConsoleLogger(NULL)

   #elif GPUT_LOG_MODE == GPUT_FILE_LOG

      #define GPUT_LOG_INIT() \
         logger_initFileLogger( \
            GPUT_LOG_FILE_NAME, GPUT_MAX_LOG_FILE_SIZE, \
            GPUT_MAX_LOG_BACKUP_FILES \
         )

   #elif GPUT_LOG_MODE == GPUT_MULTI_LOG

      #define GPUT_LOG_INIT() \
         logger_initConsoleLogger(NULL); \
         logger_initFileLogger( \
            GPUT_LOG_FILE_NAME, GPUT_MAX_LOG_FILE_SIZE, \
            GPUT_MAX_LOG_BACKUP_FILES \
         )

   #else
      #error unsupported logging mode
   #endif
#endif

#if GPUT_ACTIVE_LOG_LEVEL == GPUT_LOG_LEVEL_TRACE

   #define GPUT_LOG_TRACE(fmt, ...) \
      logger_log( \
         GPUT_LOG_LEVEL_TRACE, basename(__FILE__), __LINE__, \
         fmt __VA_OPT__(,) __VA_ARGS__ \
      )

#else
   #define GPUT_LOG_TRACE(fmt, ...)
#endif

#if GPUT_ACTIVE_LOG_LEVEL <= GPUT_LOG_LEVEL_DEBUG

   #define GPUT_LOG_DEBUG(fmt, ...) \
      logger_log( \
         GPUT_LOG_LEVEL_DEBUG, basename(__FILE__), __LINE__, \
         fmt __VA_OPT__(,) __VA_ARGS__ \
      )

#else
   #define GPUT_LOG_DEBUG(fmt, ...)
#endif

#if GPUT_ACTIVE_LOG_LEVEL <= GPUT_LOG_LEVEL_INFO

   #define GPUT_LOG_INFO(fmt, ...) \
      logger_log( \
         GPUT_LOG_LEVEL_INFO, basename(__FILE__), __LINE__, \
         fmt __VA_OPT__(,) __VA_ARGS__ \
      )

#else
   #define GPUT_LOG_INFO(fmt, ...)
#endif

#if GPUT_ACTIVE_LOG_LEVEL <= GPUT_LOG_LEVEL_WARN

   #define GPUT_LOG_WARN(fmt, ...) \
      logger_log( \
         GPUT_LOG_LEVEL_WARN, basename(__FILE__), __LINE__, \
         fmt __VA_OPT__(,) __VA_ARGS__ \
      )

#else
   #define GPUT_LOG_WARN(fmt, ...)
#endif

#if GPUT_ACTIVE_LOG_LEVEL <= GPUT_LOG_LEVEL_ERROR

   #define GPUT_LOG_ERROR(fmt, ...) \
      logger_log( \
         GPUT_LOG_LEVEL_ERROR, basename(__FILE__), __LINE__, \
         fmt __VA_OPT__(,) __VA_ARGS__ \
      )

#else
   #define GPUT_LOG_ERROR(fmt, ...)
#endif

#if GPUT_ACTIVE_LOG_LEVEL <= GPUT_LOG_LEVEL_FATAL

   #define GPUT_LOG_FATAL(fmt, ...) \
      logger_log( \
         GPUT_LOG_LEVEL_FATAL, basename(__FILE__), __LINE__, \
         fmt __VA_OPT__(,) __VA_ARGS__ \
      )

#else
   #define GPUT_LOG_FATAL(fmt, ...)
#endif

const char* getGlErrorStr(int errorCode);

#ifdef GPUT_DEBUG

   #define GPUT_ASSERT(statement, message, ...) \
   { \
      bool asserted = statement; \
      if (!asserted) { \
         GPUT_LOG_FATAL(message __VA_OPT__(,) __VA_ARGS__); \
         assert(asserted); \
      } \
   }

   #define GLC(glCall) \
      glCall; \
   { \
      GLint glErrorCode = glGetError(); \
      GPUT_ASSERT(glErrorCode == GL_NO_ERROR, \
         "Error %s en GL call %s", \
          getGlErrorStr(glErrorCode), #glCall \
      ) \
   }
#else
   #define GPUT_ASSERT(statement, message, ...)
   #define GLC(glCall)
#endif
