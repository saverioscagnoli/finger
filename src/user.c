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

List *get_online_users_logins() {
  List *logins = new_list();

  struct utmpx *ut;
  setutxent();
  while ((ut = getutxent())) {
    if (ut->ut_type == USER_PROCESS) {
      char ut_user[sizeof(ut->ut_user) + 1];
      strncpy(ut_user, ut->ut_user, sizeof(ut->ut_user));
      ut_user[sizeof(ut->ut_user)] = '\0'; // ensure null-termination
      logins->add(logins, strdup(ut_user));
    }
  }

  return logins;
}

struct passwd *getpwnam_realname(char *realname) {
  struct passwd *pw;
  setpwent(); // rewind to the beginning of the user database

  while ((pw = getpwent()) != NULL) { // read the next entry
    if (pw->pw_gecos == NULL) {
      continue; // skip this user if pw_gecos is NULL
    }

    List *tokens = split_string(strdup(pw->pw_gecos), ",");

    char *name = tokens->items[0];
    if (name == NULL) {
      continue; // skip this user if name is NULL
    }

    if (strcasecmp(name, realname) == 0) {
      endpwent(); // close the user database
      return pw;  // return the matching entry
    }

    List *name_splitted = split_string(strdup(name), " ");
    if (name_splitted == NULL) {
      continue; // skip this user if splitting failed
    }

    for (int i = 0; i < name_splitted->length; i++) {
      if (name_splitted->items[i] != NULL &&
          strcasecmp(name_splitted->items[i], realname) == 0) {
        endpwent(); // close the user database
        return pw;  // return the matching entry
      }
    }
  }

  endpwent();  // close the user database
  return NULL; // no matching entry was found
}

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

int check_nofinger(char *directory) {
  struct stat st;
  char *nofinger_path = malloc(strlen(directory) + 10);
  strcpy(nofinger_path, directory);
  strcat(nofinger_path, "/.nofinger");

  if (stat(nofinger_path, &st) == 0) {
    return 1;
  }

  return 0;
}

char *read_dotfile(char *home_dir, char *dotfile_name) {
  char *dotfile_path = malloc(strlen(home_dir) + strlen(dotfile_name) + 2);
  strcpy(dotfile_path, home_dir);
  strcat(dotfile_path, "/");
  strcat(dotfile_path, dotfile_name);

  FILE *dotfile = fopen(dotfile_path, "r");

  if (dotfile == NULL) {
    return NULL;
  }

  fseek(dotfile, 0, SEEK_END);
  long length = ftell(dotfile);
  fseek(dotfile, 0, SEEK_SET);

  char *contents = malloc(length + 1);
  fread(contents, 1, length, dotfile);
  contents[length] = '\0';

  fclose(dotfile);

  return contents;
}

User *get_user_info(char *login, int skip_usernames) {
  struct passwd *pw;
  struct utmpx *ut;
  time_t now;
  User *user = malloc(sizeof(struct User));
  user->name = NULL;
  user->login = NULL;
  user->login_time = NULL;
  user->idle_time = NULL;
  user->tty = NULL;
  user->office.number = NULL;
  user->office.phone = NULL;

  pw = getpwnam(login);

  if (pw == NULL) {
    if (skip_usernames == 0) {
      pw = getpwnam_realname(login);
    }
  }

  if (pw == NULL) {
    return NULL;
  }

  // printf("login: %s\n", login);

  user->login = strdup(pw->pw_name);
  user->directory = strdup(pw->pw_dir);

  if (check_nofinger(user->directory) == 1) {
    return NULL;
  }

  user->shell = strdup(pw->pw_shell);

  List *tokens = split_string(strdup(pw->pw_gecos), ",");

  user->name = tokens->items[0];

  if (tokens->length >= 3) {
    user->office.number = strdup(tokens->items[1]);

    if (strcmp(user->office.number, "") == 0) {
      user->office.number = NULL;
    }

    user->office.phone = strdup(tokens->items[2]);

    if (strcmp(user->office.phone, "") == 0) {
      user->office.phone = NULL;
    }

  } else if (tokens->length == 2) {
    user->office.number = strdup(tokens->items[1]);

    if (strcmp(user->office.number, "") == 0) {
      user->office.number = NULL;
    }
  }

  ut = get_user_entry(user->login);

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

      if (ut.ut_type == USER_PROCESS && !strcmp(ut_user, user->login)) {
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

  // Get the .forward file and return its contents
  user->forward = read_dotfile(user->directory, ".forward");

  // Get the .pgpkey file and return its contents
  user->pgpkey = read_dotfile(user->directory, ".pgpkey");

  // Get the .project file and return its contents
  user->project = read_dotfile(user->directory, ".project");

  // Get the .plan file and return its contents
  user->plan = read_dotfile(user->directory, ".plan");

  return user;
}
