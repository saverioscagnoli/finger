#include "headers/list.h"
#include "headers/user.h"
#include "headers/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
  int shortFlag = 0;

  User **users = malloc((argc - 1) * sizeof(User *));
  int userCount = 0;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-s") == 0) {
      shortFlag = 1;
    } else {
      users[userCount++] = get_user_info(argv[i]);
    }
  }

  if (shortFlag) {
    if (userCount == 0) {
      users[0] = get_user_info(getlogin());
      userCount++;
    }

    print_users_short(users, userCount);
  } else {
  }

  free(users);
  return 0;
}