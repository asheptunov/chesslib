#include <stdlib.h>
#include <stdio.h>

#include "arraylist.h"

void CTEST(int _condition, char *_msg, int _exit_on_fail);
void CSUMMARY();

void testMake();
void testFree();
void testPut();
void testGet();
void testAppend();
void testIndexOf();

int _tstct = 0;
int _passct = 0;
int _failct = 0;

int main(int argc, char **argv) {
  testMake();
  testFree();
  testPut();
  testGet();
  testAppend();
  testIndexOf();
  CSUMMARY();
  return 0;
}

void CTEST(int _condition, char *_msg, int _exit_on_fail) {
  ++_tstct;
  if (_condition) {
    ++_passct;
  } else {
    ++_failct;
    fprintf(stderr, "test %d failed\n", _tstct);
    if (_msg) {
      fprintf(stderr, "%s\n", _msg);
    }
    if (_exit_on_fail) {
      exit(1);
    }
  }
}

void CSUMMARY() {
  fprintf(stdout, "%d tests run; %d passed; %d failed.\n", _tstct, _passct, _failct);
}

void testMake() {
  alst_t list = alst_make(10);
  CTEST(list.cap == 10 && list.len == 0 && list.data, NULL, 1);
  list = alst_make(0);
  CTEST(list.cap == 10 && list.len == 0 && list.data, NULL, 1);
  list = alst_make(999);
  CTEST(list.cap = 999 && list.len == 0 && list.data, NULL, 1);
}

void testFree() {
  alst_t list = alst_make(10);
  aslt_free(&list);
  CTEST(1, NULL, 0);
}

void testPut() {
  alst_t list = alst_make(10);
  list.len = 1;
  alst_put(&list, 0, (void *) 0xdeadbeef);
  CTEST((list.cap == 10) && (list.len == 1) && (list.data) && (list.data[0] == (void *) 0xdeadbeef), NULL, 1);
  list.len = 10;
  alst_put(&list, 9, (void *) 0xcafef00d);
  CTEST((list.cap == 10) && (list.len == 10) && (list.data) && (list.data[9] == (void *) 0xcafef00d), NULL, 1);
  alst_put(&list, 5, (void *) 0x11111111);
  CTEST((list.cap == 10) && (list.len == 10) && (list.data) && (list.data[5] == (void *) 0x11111111), NULL, 1);
}

void testGet() {
  alst_t list = alst_make(10);
  list.len = 1;
  alst_put(&list, 0, (void *) 0xdeadbeef);
  void *val = alst_get(&list, 0);
  CTEST(val == (void *) 0xdeadbeef, NULL, 1);
  list.len = 5;
  alst_put(&list, 4, (void *) 0xcafef00d);
  val = alst_get(&list, 4);
  CTEST(val == (void *) 0xcafef00d, NULL, 1);
}

void testAppend() {
  alst_t list = alst_make(10);
  for (size_t i = 0; i < list.cap; ++i) {
    alst_append(&list, (void *) 0xdeadbeef);
  }
  char msg[100];
  CTEST((list.len == 10) && (list.cap == 10) && (list.data), msg, 1);
  int match = 1;
  for (size_t i = 0; i < list.len; ++i) {
    match &= (list.data[i] == (void *) 0xdeadbeef);
  }
  CTEST(match, NULL, 1);

  alst_append(&list, (void *) 0xcafef00d);
  CTEST((list.len == 11) && (list.cap == 100) && (list.data), NULL, 1);
  match = 1;
  for (size_t i = 0; i < 10; ++i) {
    match &= list.data[i] == (void *) 0xdeadbeef;
  }
  CTEST(match, NULL, 1);
  CTEST(list.data[10] == (void *) 0xcafef00d, NULL, 1);
}

void testIndexOf() {
  alst_t list = alst_make(10);
  for (size_t i = 0; i < list.cap; ++i) {
    alst_append(&list, (void *) 0xdeadbeef);
  }
  alst_put(&list, 8, (void *) 0xcafef00d);
  CTEST(alst_index_of(&list, (void *) 0xcafef00d) == 8u, NULL, 1);
  alst_put(&list, 8, (void *) 0xdeadbeef);
  alst_put(&list, 9, (void *) 0xcafef00d);
  CTEST(alst_index_of(&list, (void *) 0xcafef00d) == 9, NULL, 1);
  alst_put(&list, 2, (void *) 0xcafef00d);
  CTEST(alst_index_of(&list, (void *) 0xcafef00d) == 2, NULL, 1);
  CTEST(list.len == 10 && list.cap == 10 && list.data, NULL, 1);
}
