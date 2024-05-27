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

typedef struct Office {
  char *number;
  char *phone;
} Office;

typedef struct User {
  char *name;
  char *login;
  char *login_time;
  char *idle_time;
  char *tty;
  Office office;
} User;

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

struct User *get_user_info(char *login) {
  struct passwd *pw;
  struct utmpx *ut;
  time_t now;
  struct User *user = malloc(sizeof(struct User));

  pw = getpwnam(login);

  if (pw) {
    user->login = strdup(pw->pw_name);

    List *tokens = split(pw->pw_gecos, ",");

    user->name = tokens->items[0];

    user->office.number = "*";
    user->office.phone = "*";

    if (tokens->items[1]) {
      user->office.number = tokens->items[1];
    } else {
      user->office.number = "*";
    }

    if (tokens->items[2]) {
      user->office.phone = tokens->items[2];
    } else {
      user->office.phone = "*";
    }

    struct utmpx *ut_entry = get_user_entry(login);

    if (ut_entry) {

      time_t login_time = ut_entry->ut_tv.tv_sec;
      time_t cloned_time = login_time;

      user->login_time = format_time(login_time, false);
      now = time(NULL);
      user->idle_time = format_time(now - cloned_time, true);
      user->tty = ut_entry->ut_line;
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
          user->idle_time = "*";
          user->tty = ut.ut_line;
          found = true;
          break;
        }
      }

      if (read_size == -1) {
        return NULL;
      }

      if (!found) {
        user->login_time = "No logins";
        user->idle_time = "*";
        user->tty = "*";
      }

      close(fd);
    }

  } else {
    return NULL;
  }

  return user;
}