#include "headers/list.h"
#include "headers/user.h"
#include "headers/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int shortFlag = 0;
  int longFlag = 0;

  int userCount = 0;

  if (argc == 1 || (argc == 2 && strcmp(argv[1], "-l") == 0)) {
    List *logins = get_online_users_logins();

    User **users = malloc(logins->length * sizeof(User *));

    for (int i = 0; i < logins->length; i++) {
      User *user = get_user_info(logins->items[i]);
      if (user != NULL) {
        users[userCount] = user;
        userCount++;
      }
    }

    if (argc == 1) {
      print_users_short(users, userCount);
    } else {
      print_users_long(users, userCount);
    }

    return 0;
  }

  User **users = malloc((argc - 1) * sizeof(User *));

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-s") == 0) {
      shortFlag = 1;
      longFlag = 0;
    } else if (strcmp(argv[i], "-l") == 0) {
      shortFlag = 0;
      longFlag = 1;
    } else {
      User *user = get_user_info(argv[i]);

      if (user != NULL) {
        users[userCount] = user;
        userCount++;
      } else {
        fprintf(stderr, "Error: user %s not found\n", argv[i]);
      }
    }
  }

  if (shortFlag) {
    if (userCount == 0) {
      users[0] = get_user_info(getlogin());
      userCount++;
    }

    print_users_short(users, userCount);
  } else if (longFlag || (!shortFlag && !longFlag)) {
    if (userCount == 0) {
      users[0] = get_user_info(getlogin());
      userCount++;
    }

    print_users_long(users, userCount);
  }

  free(users);
  return 0;
}