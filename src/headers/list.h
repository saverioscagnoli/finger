#ifndef LIST_H
#define LIST_H

typedef struct List List;

struct List {
  char **items;
  int length;

  void (*add)(List *, char *);
  void (*remove_item)(List *, int);
};

List *new_list();
List *split(char *str, char *delim);

#endif // LIST_H