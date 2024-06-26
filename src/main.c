#include "headers/list.h"
#include "headers/user.h"
#include "headers/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int short_format = 0;
  int long_format = 0;
  int hide_dotfiles = 0;
  int skip_usernames = 0;

  int userCount = 0;

  if (argc == 1 || ((argc == 2 && strcmp(argv[1], "-l") == 0) ||
                    (argc == 2 && strcmp(argv[1], "-s") == 0))) {
    List *logins = get_online_users_logins();

    User **users = malloc(logins->length * sizeof(User *));

    for (int i = 0; i < logins->length; i++) {
      User *user = get_user_info(logins->items[i], 0);

      if (user != NULL) {
        users[userCount] = user;
        userCount++;
      }
    }

    if (argc == 1 || (argc == 2 && strcmp(argv[1], "-s") == 0)) {
      print_users_short(users, userCount);
    } else {
      print_users_long(users, userCount, 0);
    }

    return 0;
  }

  User **users = malloc((argc - 1) * sizeof(User *));
  User **seen_users = malloc((argc - 1) * sizeof(User *));

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-s") == 0) {
      short_format = 1;
      long_format = 0;
    } else if (strcmp(argv[i], "-l") == 0) {
      short_format = 0;
      long_format = 1;
    } else if (strcmp(argv[i], "-p") == 0) {
      hide_dotfiles = 1;
    } else if (strcmp(argv[i], "-m") == 0) {
      skip_usernames = 1;
    } else {
      User *user = get_user_info(argv[i], skip_usernames);

      int already_seen = 0;

      // Check if the user was inputted multiple times
      if (user != NULL) {
        for (int j = 0; j < userCount; j++) {
          if (users[j] != NULL && strcmp(users[j]->login, user->login) == 0) {
            already_seen = 1;
            break;
          }
        }
      }

      // Check if the user is not NULL, and if it was not already seen
      if (user != NULL && already_seen == 0) {
        users[userCount] = user;
        userCount++;
      } else if (user == NULL) {
        fprintf(stderr, "Error: user %s not found\n", argv[i]);
      }
    }
  }

  if (userCount != 0) {
    if (short_format) {
      print_users_short(users, userCount);
    } else if (long_format || (!short_format && !long_format)) {
      print_users_long(users, userCount, hide_dotfiles);
    }
  }

  free(users);
  return 0;
}