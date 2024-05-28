#ifndef USER_H
#define USER_H

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

User *get_user_info(char *login);

#endif // USER_H