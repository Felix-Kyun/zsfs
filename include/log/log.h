#ifndef __ZSFS_LOGGER__
#define __ZSFS_LOGGER__

/*
 * simple logging library for internal use in zsfs
 * its intended to be pretty basic
 */

// #define log log_ // to avoid conflict with log() from math.h
#define ok(message) log_(LOG_SUCCESS, message)
#define done() ok("done")
#define err(message) log_(LOG_ERROR, message)
#define info(message) log_(LOG_INFO, message)

#include <stdio.h>

#define COLOR_RESET "\x1b[0m"
#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_BLUE "\x1b[34m"

typedef enum LogLevel {
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_SUCCESS,
    LOG_MAX // keep it at the end 
} LogLevel;

void log_(LogLevel level, const char *message);

void Log(const LogLevel lv, const char *fmt, ...);

#endif 
