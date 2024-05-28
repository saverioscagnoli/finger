#include "headers/user.h"
#include "headers/list.h"
#include "headers/utils.h"
#include <fcntl.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <utmp.h>
#include <utmpx.h>

struct utmpx *get_user_entry(char *login) {
  struct utmpx *ut;
  setutxent();
  while ((ut = getutxent())) {
    if (ut->ut_type == USER_PROCESS) {
      char ut_user[sizeof(ut->ut_user) + 1];
      strncpy(ut_user, ut->ut_user, sizeof(ut->ut_user));
      ut_user[sizeof(ut->ut_user)] = '\0'; // ensure null-termination
      if (!strcmp(ut_user, login)) {
        return ut;
      }
    }
  }
  return NULL;
}

User *get_user_info(char *login) {
  struct passwd *pw;
  struct utmpx *ut;
  time_t now;
  User *user = malloc(sizeof(struct User));

  user->name = "*";
  user->login = "*";
  user->login_time = "*";
  user->idle_time = "*";
  user->tty = "*";
  user->office.number = "*";
  user->office.phone = "*";

  if (user == NULL) {
    return NULL;
  }

  pw = getpwnam(login);

  if (pw == NULL) {
    return NULL;
  }

  user->login = strdup(pw->pw_name);
  List *tokens = split(pw->pw_gecos, ",");

  user->name = tokens->items[0];

  if (tokens->items[1]) {
    user->office.number = tokens->items[1];
  }

  if (tokens->items[2]) {
    user->office.phone = tokens->items[2];
  }

  ut = get_user_entry(login);

  if (ut) {
    user->login_time = format_time(ut->ut_tv.tv_sec, false);
    now = time(NULL);
    user->idle_time = format_time(now - ut->ut_tv.tv_sec, true);
    user->tty = ut->ut_line;
  } else {
    // User not logged in
    // Get the last login time from /var/log/wtmp

    int fd = open("/var/log/wtmp", O_RDONLY);

    if (fd == -1) {
      return NULL;
    }

    struct utmpx ut;
    ssize_t read_size;

    int found = false;

    while ((read_size = read(fd, &ut, sizeof(struct utmpx))) ==
           sizeof(struct utmpx)) {
      char ut_user[__UT_NAMESIZE + 1] = {0}; // +1 for the null terminator
      strncpy(ut_user, ut.ut_user, __UT_NAMESIZE);

      if (ut.ut_type == USER_PROCESS && !strcmp(ut_user, login)) {
        user->login_time = format_time(ut.ut_tv.tv_sec, false);
        char tty[sizeof(ut.ut_line) + 1];
        memcpy(tty, ut.ut_line, sizeof(ut.ut_line));
        tty[sizeof(ut.ut_line)] = '\0';
        user->tty = strdup(tty);
        found = true;
        break;
      }
    }

    if (read_size == -1) {
      return NULL;
    }

    if (!found) {
      user->login_time = "No logins";
    }

    close(fd);
  }

  return user;
}