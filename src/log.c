#include "../include/log/log.h"
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

void log(LogLevel level, const char *message) {
  // level and color map
  char *color_code[LOG_MAX] = {COLOR_BLUE, COLOR_YELLOW, COLOR_RED,
                               COLOR_GREEN};
  // char *level_name[LOG_MAX] = {"INFO", "WARNING", "ERROR", "SUCCESS"};
  char *level_name[LOG_MAX] = {"I", "W", "E", "S"};

  // get time
  time_t current_time = time(NULL);
  struct tm *t = localtime(&current_time);
  char formatted_time[128];
  strftime(formatted_time, 128, "%d-%m-%Y %H:%M:%S", t);

  printf("%s[%s] %s %s%s\n", color_code[level], formatted_time,
         level_name[level], message, COLOR_RESET);
  return;
}

void Log(const LogLevel lv, const char *fmt, ...) {
  // level and color map
  char *color_code[LOG_MAX] = {COLOR_BLUE, COLOR_YELLOW, COLOR_RED,
                               COLOR_GREEN};
  // char *level_name[LOG_MAX] = {"INFO", "WARNING", "ERROR", "SUCCESS"};
  char *level_name[LOG_MAX] = {"I", "W", "E", "S"};

  // get time
  time_t current_time = time(NULL);
  struct tm *t = localtime(&current_time);
  char formatted_time[128];
  strftime(formatted_time, 128, "%d-%m-%Y %H:%M:%S", t);

  printf("");
  printf("%s[%s] %s ", color_code[lv], formatted_time, level_name[lv]);

  // handle varg
  va_list arg;
  va_start(arg, fmt);

  vprintf(fmt, arg);

  va_end(arg);

  // reset color
  puts(COLOR_RESET);
  return;
}
