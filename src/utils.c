#include "headers/user.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

int max(int a, int b) { return (a > b) ? a : b; }

char *get_timezone_name() {
  tzset();
  return __tzname[0];
}

void get_user_values_short(User *user, char *values[]) {
  values[0] = user->login;
  values[1] = user->name;
  values[2] = user->tty;
  values[3] = user->idle_time;
  values[4] = user->login_time;
  values[5] = user->office.number;
  values[6] = user->office.phone;
}

#define PADDING 3

void print_users_short(struct User **users, int num_users) {
  char *headers[] = {"Login",      "Name",   "Tty",         "Idle",
                     "Login Time", "Office", "Office Phone"};

  int max_lengths[7] = {0};

  char *user_values[num_users][7];

  for (int i = 0; i < num_users; i++) {
    if (users[i] == NULL) {
      continue;
    }

    get_user_values_short(users[i], user_values[i]);

    for (int j = 0; j < 7; j++) {
      max_lengths[j] = max(max_lengths[j], strlen(user_values[i][j]));
    }
  }

  for (int i = 0; i < 7; i++) {
    max_lengths[i] = max(max_lengths[i], strlen(headers[i]));
  }

  for (int i = 0; i < 7; i++) {
    printf("%-*s", max_lengths[i] + PADDING, headers[i]);
  }

  printf("\n");

  for (int i = 0; i < num_users; i++) {
    for (int j = 0; j < 7; j++) {
      printf("%-*s", max_lengths[j] + PADDING, user_values[i][j]);
    }

    printf("\n");
  }
}

void print_users_long(struct User **users, int num_users) {
  printf("Login: %s\n", users[0]->login);
  printf("Directory: %s\n", users[0]->directory);

  printf("Office: %s, %s\n", users[0]->office.number, users[0]->office.phone);

  printf("%s %s (%s) on %s",
         users[0]->logged_in == 1 ? "On since" : "Last login",
         users[0]->login_time, get_timezone_name(), users[0]->tty);

  printf("\n");
}