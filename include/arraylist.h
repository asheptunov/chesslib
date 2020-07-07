#pragma once

#include "stdlib.h"

/**
* An arraylist of (len) values, capacity for (cap) values, and an array (**data) of size (sizeof(void*)) data members.
*/
typedef struct {
  size_t len;  // length
  size_t cap;  // capacity
  void **data;  // the array
} alst_t;

/**
* Returns a new arraylist with at least the specified capacity.
*/
alst_t *alst_make(size_t cap);

/**
* Frees an arraylist and all associated data, optionally freeing each of the (len) members using the freeing function
* (free_func). If (free_func) is NULL, will treat members as raw values and not free them.
*/
void alst_free(alst_t *list, void (*free_func)(void *));

/**
* Puts the value (val) at the index (i) in the arraylist. Index must be in range [0, list->len).
*/
void alst_put(alst_t *list, size_t i, void *val);

/**
* Returns the value at the index (i) in the arraylist. Index must be in range [0, list->len).
*/
void *alst_get(alst_t *list, size_t i);

/**
* Appends the value (val) to the arraylist. Subsequent calls to alst_get(list, list->len-1) will return (val).
*/
void alst_append(alst_t *list, void *val);
