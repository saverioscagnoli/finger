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

      max_lengths[j] =
          max(max_lengths[j],
              strlen(user_values[i][j] == NULL ? "*" : user_values[i][j]));
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

      printf("%-*s", max_lengths[j] + PADDING,
             user_values[i][j] == NULL ? "*" : user_values[i][j]);
    }

    printf("\n");
  }
}

void print_users_long(struct User **users, int num_users, int hide_dotfiles) {

  for (int i = 0; i < num_users; i++) {
    User *user = users[i];

    if (user == NULL) {
      continue;
    }

    printf("Login: %s\n", user->login);
    printf("Name: %s\n", user->name);
    printf("Directory: %s\n", user->directory);
    printf("Shell: %s\n", user->shell);

    if (user->office.phone != NULL && user->office.number != NULL) {
      printf("Office: %s, %s\n", user->office.number, user->office.phone);
    } else if (user->office.number != NULL) {
      printf("Office: %s\n", user->office.number);
    } else if (user->office.phone != NULL) {
      printf("Office Phone: %s\n", user->office.phone);
    }

    if (user->logged_in) {
      printf("On since %s on %s\n", user->login_time, user->tty);
      printf("  %s Idle\n", user->idle_time);
    } else {
      printf("Last login %s on %s\n", user->login_time, user->tty);
    }

    if (hide_dotfiles == 1) {
      return;
    }

    if (user->forward != NULL) {
      printf("Mail forwarded to %s", user->forward);
    }

    printf("No mail.\n");

    if (user->pgpkey != NULL) {
      printf("PGP key:\n%s", user->pgpkey);
    }

    if (user->project != NULL) {
      printf("Project:\n%s", user->project);
    }

    if (user->plan == NULL) {
      printf("No Plan.\n");
    } else {
      printf("Plan:\n%s", user->plan);
    }

    if (num_users > 1) {
      printf("\n");
    }
  }
}