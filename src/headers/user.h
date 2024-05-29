#ifndef USER_H
#define USER_H

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
} User;

User *get_user_info(char *login);

#endif // USER_H