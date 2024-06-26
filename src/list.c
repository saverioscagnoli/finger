#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct List {
  char **items;
  int length;

  void (*add)(struct List *, char *);
  void (*remove_item)(struct List *, int);
  int (*includes)(struct List *, char *);
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

int includes(List *list, char *item) {
  for (int i = 0; i < list->length; i++) {
    if (strcmp(list->items[i], item) == 0) {
      return 1;
    }
  }

  return 0;
}

List *new_list() {
  List *list = malloc(sizeof(List));
  list->items = malloc(sizeof(char *));
  list->length = 0;

  list->add = add;
  list->remove_item = remove_item;

  return list;
}

List *split_string(char *str, char *delim) {
  List *list = new_list();

  char *start = str;
  char *end = NULL;
  char *temp = NULL;

  // Loop until no more delimiters are found
  while ((end = strchr(start, delim[0])) != NULL) {
    // Allocate memory for the new string
    temp = (char *)malloc(end - start + 1);

    // Copy the substring into the new string
    memcpy(temp, start, end - start);

    // Null-terminate the new string
    temp[end - start] = '\0';

    list->add(list, temp);

    start = end + 1;

    // If there are consecutive delimiters, add an empty string to the list for
    // each one
    while (*start == delim[0]) {
      list->add(list, "\0");
      start++;
    }
  }

  // If there are any characters after the last delimiter, add these to the list
  // as a new string
  if (*start) {
    list->add(list, strdup(start));
  }

  // Return the list of split strings
  return list;
}