extern "C" {
#include "arraylist.h"
}

#include <gtest/gtest.h>
#include <cstdlib>
#include <iostream>

TEST(ArrayListTest, Make) {
  alst_t *list = alst_make(10);
  EXPECT_EQ(list->cap, 10);
  EXPECT_EQ(list->len, 0);
  EXPECT_TRUE(list->data);
  alst_free(list);
  list = alst_make(0);
  EXPECT_EQ(list->cap, 10);
  EXPECT_EQ(list->len, 0);
  EXPECT_TRUE(list->data);
  alst_free(list);
  list = alst_make(999);
  EXPECT_EQ(list->cap, 999);
  EXPECT_EQ(list->len, 0);
  EXPECT_TRUE(list->data);
  alst_free(list);
}

TEST(ArrayListTest, Free) {
  alst_t *list = alst_make(10);
  alst_free(list);
  EXPECT_TRUE(1);
}

TEST(ArrayListTest, Put) {
  alst_t *list = alst_make(10);
  list->len = 1;
  alst_put(list, 0, (void *) 0xdeadbeef);
  EXPECT_EQ(list->cap, 10);
  EXPECT_EQ(list->len, 1);
  EXPECT_TRUE(list->data);
  EXPECT_EQ(list->data[0], (void *) 0xdeadbeef);
  list->len = 10;
  alst_put(list, 9, (void *) 0xcafef00d);
  EXPECT_EQ(list->cap, 10);
  EXPECT_EQ(list->len, 10);
  EXPECT_TRUE(list->data);
  EXPECT_EQ(list->data[9], (void *) 0xcafef00d);
  alst_put(list, 5, (void *) 0x11111111);
  EXPECT_EQ(list->cap, 10);
  EXPECT_EQ(list->len, 10);
  EXPECT_TRUE(list->data);
  EXPECT_EQ(list->data[5], (void *) 0x11111111);
  alst_free(list);
}

TEST(ArrayListTest, Get) {
  alst_t *list = alst_make(10);
  list->len = 1;
  alst_put(list, 0, (void *) 0xdeadbeef);
  void *val = alst_get(list, 0);
  EXPECT_EQ(val, (void *) 0xdeadbeef);
  list->len = 5;
  alst_put(list, 4, (void *) 0xcafef00d);
  val = alst_get(list, 4);
  EXPECT_EQ(val, (void *) 0xcafef00d);
  alst_free(list);
}

TEST(ArrayListTest, Append) {
  alst_t *list = alst_make(10);
  for (size_t i = 0; i < list->cap; ++i) {
    alst_append(list, (void *) 0xdeadbeef);
  }
  EXPECT_EQ(list->len, 10);
  EXPECT_EQ(list->cap, 10);
  EXPECT_TRUE(list->data);
  int match = 1;
  for (size_t i = 0; i < list->len; ++i) {
    match &= (list->data[i] == (void *) 0xdeadbeef);
  }
  EXPECT_TRUE(match);

  alst_append(list, (void *) 0xcafef00d);
  EXPECT_EQ(list->len, 11);
  EXPECT_EQ(list->cap, 100);
  EXPECT_TRUE(list->data);
  match = 1;
  for (size_t i = 0; i < 10; ++i) {
    match &= list->data[i] == (void *) 0xdeadbeef;
  }
  EXPECT_TRUE(match);
  EXPECT_EQ(list->data[10], (void *) 0xcafef00d);
  alst_free(list);
}
