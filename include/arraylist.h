#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include "stdlib.h"

// an arraylist of len values of size void*
typedef struct {
  size_t len;  // length
  size_t cap;  // capacity
  void **data;  // the array
} alst_t;

// returns an arraylist of the designated capacity cap, with data zeroed if zero specified
alst_t *alst_make(size_t cap);

// frees all data associated with the arraylist list
void alst_free(alst_t *list);

// puts the value val at the index i in the arraylist list
void alst_put(alst_t *list, size_t i, void *val);

// returns the value at the index i in the arraylist list
void *alst_get(alst_t *list, size_t i);

// appends the value val at the end of the arraylist list
void alst_append(alst_t *list, void *val);

#endif  // ARRAYLIST_H
