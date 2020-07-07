#pragma once

#include "stdlib.h"

// an arraylist of len values of size void*
typedef struct {
  size_t len;  // length
  size_t cap;  // capacity
  void **data;  // the array
} alst_t;

// returns an arraylist of the designated capacity cap (true capacity will be max(10, cap))
alst_t *alst_make(size_t cap);

// frees all data associated with the arraylist list, given the specified (optionally NULL) element freeing function
void alst_free(alst_t *list, void (*free_func)(void *));

// puts the value val at the index i in the arraylist list
void alst_put(alst_t *list, size_t i, void *val);

// returns the value at the index i in the arraylist list
void *alst_get(alst_t *list, size_t i);

// appends the value val at the end of the arraylist list
void alst_append(alst_t *list, void *val);
