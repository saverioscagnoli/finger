#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct List {
  char **items;
  int length;

  void (*add)(struct List *, char *);
  void (*remove_item)(struct List *, int);
} List;

void add(List *list, char *item) {
  list->items = realloc(list->items, (list->length + 1) * sizeof(char *));
  list->items[list->length] = item;
  list->length++;
}

void remove_item(List *list, int index) {
  if (index < 0 || index >= list->length) {
    return;
  }

  for (int i = index; i < list->length - 1; i++) {
    list->items[i] = list->items[i + 1];
  }

  list->length--;
  list->items = realloc(list->items, list->length * sizeof(char *));
}

List *new_list() {
  List *list = malloc(sizeof(List));
  list->items = malloc(sizeof(char *));
  list->length = 0;

  list->add = add;
  list->remove_item = remove_item;

  return list;
}

List *split(char *str, char *delim) {
  List *list = new_list();
  char *token = strtok(str, delim);

  while (token != NULL) {
    list->add(list, token);
    token = strtok(NULL, delim);
  }

  list->add(list, "\0");

  return list;
}