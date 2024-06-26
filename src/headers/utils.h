#ifndef UTILS_H
#define UTILS_H

#include "user.h"
#include <stdbool.h>
#include <time.h>

#define PADDING 3

char *format_time(time_t time, bool short_format);

int max(int a, int b);

void print_users_short(struct User **users, int num_users);

void print_users_long(struct User **users, int num_users, int hide_dotfiles);

#endif // UTILS_H