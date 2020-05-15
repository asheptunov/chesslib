#include "stdio.h"
#include "string.h"

#include "arraylist.h"

alst_t alst_make(size_t cap) {
  alst_t ret;
  if (cap <= 0) {
    cap = 10;
  }
  ret.len = 0;
  ret.cap = cap;
  ret.data = malloc(cap * sizeof(void *));
  if (!ret.data) {
    fprintf(stderr, "alst_make malloc failure\n");
    exit(1);
  }
  return ret;
}

void aslt_free(alst_t *list) {
  free(list->data);
}

void alst_put(alst_t *list, size_t i, void *val) {
  if (i > list->len - 1) {  // check for iiob
    fprintf(stderr, "alst_put bad index %lud for alst with len %lud\n", i, list->len);
    exit(1);
  }
  list->data[i] = val;
}

void *alst_get(alst_t *list, size_t i) {
  if (i > list->len - 1) {
    fprintf(stderr, "alst_put bad index %lud for alst with len %lud\n", i, list->len);
    exit(1);
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
      exit(1);
    }
    memcpy(list->data, old, list->len * sizeof(void *));
    free(old);
  }
  list->data[list->len++] = val;
}

size_t alst_index_of(alst_t *list, void *val) {
  for (size_t i = 0; i < list->len; ++i) {
    if (list->data[i] == val) {
      return i;
    }
  }
  return -1;
}
