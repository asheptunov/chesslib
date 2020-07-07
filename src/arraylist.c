#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "arraylist.h"

alst_t *alst_make(size_t cap) {
  alst_t *ret = (alst_t *) malloc(sizeof(alst_t));
  if (!ret) {
    fprintf(stderr, "alst_make malloc failure 1\n");
    exit(EXIT_FAILURE);
  }
  ret->len = 0;
  ret->cap = (cap < 10) ? 10 : cap;
  ret->data = malloc(cap * sizeof(void *));
  if (!(ret->data)) {
    fprintf(stderr, "alst_make malloc failure 2\n");
    exit(EXIT_FAILURE);
  }
  return ret;
}

void alst_free(alst_t *list, void (*free_func)(void *)) {
  if (!free_func) {
    goto ALST_FREE_DONE;
  }
  // free all the elements
  for (size_t i = 0; i < list->len; ++i) {
    free_func(list->data[i]);
  }
ALST_FREE_DONE:
  // free the array
  free(list->data);
  free(list);
}

void alst_put(alst_t *list, size_t i, void *val) {
  if (i > list->len - 1) {  // check for iiob
    fprintf(stderr, "alst_put bad index %zu for alst with len %zu\n", i, list->len);
    exit(EXIT_FAILURE);
  }
  list->data[i] = val;
}

void *alst_get(alst_t *list, size_t i) {
  if (i > list->len - 1) {
    fprintf(stderr, "alst_put bad index %zu for alst with len %zu\n", i, list->len);
    exit(EXIT_FAILURE);
  }
  return list->data[i];
}

void alst_append(alst_t *list, void *val) {
  if (list->len == list->cap) {  // expand
    list->cap *= 10;  // factor of 10
    void **old = list->data;
    list->data = malloc(list->cap * sizeof(void *));
    if (!(list->data)) {
      fprintf(stderr, "alst_append malloc failure\n");
      exit(EXIT_FAILURE);
    }
    memcpy(list->data, old, list->len * sizeof(void *));
    free(old);
  }
  list->data[list->len++] = val;
}
