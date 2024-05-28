#include "headers/list.h"
#include "headers/user.h"
#include "headers/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {

  if (argc == 1) {
    printf("Usage: %s --flags <login>\n", argv[0]);
    return 1;
  }

  if (strcmp(argv[1], "-s") == 0) {
    if (argc == 2) {
      char *login = getlogin();
      User *user = get_user_info(login);
      User *users[] = {user};

      print_users_short(users, 1);

      return 0;
    }

    User **users = malloc((argc - 2) * sizeof(User *));

    for (int i = 2; i < argc; i++) {
      users[i - 2] = get_user_info(argv[i]);
    }

    print_users_short(users, argc - 2);
  }

  return 0;
}