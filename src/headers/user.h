#ifndef USER_H
#define USER_H

#include "list.h"

typedef struct Office {
  char *number;
  char *phone;
} Office;

typedef struct User {
  char *name;
  int logged_in;
  char *login;
  char *login_time;
  char *idle_time;
  char *tty;
  char *directory;
  char *shell;
  Office office;

  char *forward;
  char *pgpkey;
  char *plan;
  char *project;
} User;

List *get_online_users_logins();
User *get_user_info(char *login, int skip_usernames);

#endif // USER_H