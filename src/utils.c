#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char *format_time(time_t time, bool short_format) {
  struct tm *time_info = localtime(&time);
  char *time_str = malloc(20 * sizeof(char));

  if (time_str == NULL) {
    return NULL;
  }

  if (short_format) {
    strftime(time_str, 20, "%H:%M", time_info);
  } else {
    strftime(time_str, 20, "%a %b %d %H:%M", time_info);
  }

  return time_str;
}