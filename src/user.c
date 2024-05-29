#include "headers/user.h"
#include "headers/list.h"
#include "headers/utils.h"
#include <fcntl.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
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

char *get_idle_time(char *tty) {
  struct stat tty_stat;
  char tty_path[32];
  char *idle_time_str =
      malloc(6 * sizeof(char)); //  5 characters + null terminator

  // Prepend /dev/ to the tty name
  snprintf(tty_path, sizeof(tty_path), "/dev/%s", tty);

  if (stat(tty_path, &tty_stat) == -1) {
    perror("stat");
    return NULL;
  }

  time_t now = time(NULL);
  int idle_time = difftime(now, tty_stat.st_atime) / 60;

  if (idle_time > 60) {
    int hours = idle_time / 60;
    int minutes = idle_time % 60;
    sprintf(idle_time_str, "%dh %dm", hours, minutes);

    return idle_time_str;
  }

  if (idle_time > 3600 * 24) {
    int days = idle_time / (3600 * 24);
    int hours = (idle_time % (3600 * 24)) / 3600;
    sprintf(idle_time_str, "%dd %dh", days, hours);

    return idle_time_str;
  }

  sprintf(idle_time_str, "%d", idle_time);

  return idle_time_str;
}

User *get_user_info(char *login) {
  struct passwd *pw;
  struct utmpx *ut;
  time_t now;
  User *user = malloc(sizeof(struct User));

  user->name = "";
  user->login = "";
  user->login_time = "";
  user->idle_time = "*";
  user->tty = "*";
  user->office.number = "";
  user->office.phone = "";

  if (user == NULL) {
    return NULL;
  }

  pw = getpwnam(login);

  if (pw == NULL) {
    return NULL;
  }

  user->login = strdup(pw->pw_name);

  user->directory = strdup(pw->pw_dir);
  user->shell = strdup(pw->pw_shell);

  List *tokens = split_string(strdup(pw->pw_gecos), ",");

  user->name = tokens->items[0];

  if (tokens->length >= 3) {
    user->office.number = strdup(tokens->items[1]);
    user->office.phone = strdup(tokens->items[2]);

  } else if (tokens->length == 2) {
    user->office.number = strdup(tokens->items[1]);
    user->office.phone = "*";
  }

  ut = get_user_entry(login);

  if (ut) {
    user->logged_in = 1;
    user->login_time = format_time(ut->ut_tv.tv_sec, false);

    // Create a null-terminated string from ut->ut_line
    char ut_line[__UT_LINESIZE + 1];
    strncpy(ut_line, ut->ut_line, __UT_LINESIZE);
    ut_line[__UT_LINESIZE] = '\0';

    user->tty = strdup(ut_line);
    user->idle_time = get_idle_time(strdup(user->tty));

  } else {
    // User not logged in
    // Get the last login time from /var/log/wtmp

    user->logged_in = 0;

    int fd = open("/var/log/wtmp", O_RDONLY);

    if (fd == -1) {
      return NULL;
    }

    struct utmpx ut;
    ssize_t read_size;

    int found = false;
    time_t latest_time = 0;

    while ((read_size = read(fd, &ut, sizeof(struct utmpx))) ==
           sizeof(struct utmpx)) {
      char ut_user[__UT_NAMESIZE + 1] = {0}; // +1 for the null terminator
      strncpy(ut_user, ut.ut_user, __UT_NAMESIZE);

      if (ut.ut_type == USER_PROCESS && !strcmp(ut_user, login)) {
        if (ut.ut_tv.tv_sec > latest_time) {
          latest_time = ut.ut_tv.tv_sec;
          user->login_time = format_time(latest_time, false);
          char tty[sizeof(ut.ut_line) + 1];
          memcpy(tty, ut.ut_line, sizeof(ut.ut_line));
          tty[sizeof(ut.ut_line)] = '\0';
          user->tty = strdup(tty);
          found = true;
        }
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