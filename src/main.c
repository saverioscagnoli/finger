#include "headers/list.h"
#include "headers/user.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define PADDING 2

int max(int a, int b) { return (a > b) ? a : b; }

int main(int argc, char **argv) {

  if (argc == 1) {
    printf("Usage: %s <login>\n", argv[0]);
    return 1;
  }

  if (argc == 2 && strcmp(argv[1], "-s") == 0) {
    char *login = getlogin();
    User *user = get_user_info(login);

    if (user == NULL) {
      printf("User not found\n");
      return 1;
    }

    char *headers[] = {"Login",      "Name",   "TTY",         "Idle",
                       "Login Time", "Office", "Office Phone"};
    char *values[] = {user->login,       user->name,       user->tty,
                      user->idle_time,   user->login_time, user->office.number,
                      user->office.phone};

    int max_lengths[7];

    for (int i = 0; i < 7; i++) {
      max_lengths[i] = max(strlen(headers[i]), strlen(values[i])) + PADDING;
    }

    for (int i = 0; i < 7; i++) {
      printf("%-*s ", max_lengths[i], headers[i]);
    }
    printf("\n");

    for (int i = 0; i < 7; i++) {
      printf("%-*s ", max_lengths[i], values[i]);
    }
    printf("\n");
  } else if (argc == 3 && strcmp(argv[1], "-s") == 0) {
    User *user = get_user_info(argv[2]);

    if (user == NULL) {
      printf("User not found\n");
      return 1;
    }

    char *headers[] = {"Login",      "Name",   "TTY",         "Idle",
                       "Login Time", "Office", "Office Phone"};
    char *values[] = {user->login,       user->name,       user->tty,
                      user->idle_time,   user->login_time, user->office.number,
                      user->office.phone};

    int max_lengths[7];

    for (int i = 0; i < 7; i++) {
      max_lengths[i] = max(strlen(headers[i]), strlen(values[i])) + PADDING;
    }

    for (int i = 0; i < 7; i++) {
      printf("%-*s ", max_lengths[i], headers[i]);
    }
    printf("\n");

    for (int i = 0; i < 7; i++) {
      printf("%-*s ", max_lengths[i], values[i]);
    }
    printf("\n");
  } else {
    printf("Usage: %s <login> [-s]\n", argv[0]);
    return 1;
  }

  return 0;
}