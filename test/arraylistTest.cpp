extern "C" {
#include "arraylist.h"
}

#include <gtest/gtest.h>
#include <cstdlib>
#include <iostream>

TEST(ArrayListTest, Make) {
  // make using default settings
  alst_t *list = alst_make(10);
  EXPECT_EQ(list->cap, 10);
  EXPECT_EQ(list->len, 0);
  EXPECT_TRUE(list->data);
  alst_free(list, NULL);

  // make without capacity, expect cap to be max(10, cap)
  list = alst_make(0);
  EXPECT_EQ(list->cap, 10);
  EXPECT_EQ(list->len, 0);
  EXPECT_TRUE(list->data);
  alst_free(list, NULL);

  // make with large capacity, expect appropriate allocation
  list = alst_make(999);
  EXPECT_EQ(list->cap, 999);
  EXPECT_EQ(list->len, 0);
  EXPECT_TRUE(list->data);
  alst_free(list, NULL);
}

TEST(ArrayListTest, FreeNoFunc) {
  alst_t *list = alst_make(10);
  
  // free without members (no freeing function supplied)
  alst_free(list, NULL);

  // no crash
  EXPECT_TRUE(1);
}

TEST(ArrayListTest, FreeFunc) {
  alst_t *list = alst_make(10);

  // add one member to be freed
  int *e = (int *) malloc(sizeof(int));
  if (!e) {
    std::cerr << "malloc error in ArrayListTest.FreeFunc" << std::endl;
    exit(1);
  }
  alst_append(list, e);

  // free with standard freeing function (stdlib.h free)
  alst_free(list, free);

  // no crash
  EXPECT_TRUE(1);
}

TEST(ArrayListTest, Put) {
  alst_t *list = alst_make(10);

  // add one member
  list->len = 1;
  alst_put(list, 0, (void *) 0xdeadbeef);
  EXPECT_EQ(list->cap, 10);
  EXPECT_EQ(list->len, 1);
  EXPECT_TRUE(list->data);
  // expect it to be there
  EXPECT_EQ(list->data[0], (void *) 0xdeadbeef);

  // glassbox the alst by expanding its length, inserting a member at the end
  list->len = 10;
  alst_put(list, 9, (void *) 0xcafef00d);
  EXPECT_EQ(list->cap, 10);
  EXPECT_EQ(list->len, 10);
  EXPECT_TRUE(list->data);
  // expect it to be there
  EXPECT_EQ(list->data[9], (void *) 0xcafef00d);
  // expect the other member to still be there
  EXPECT_EQ(list->data[0], (void *) 0xdeadbeef);

  // insert a member in the middle
  alst_put(list, 5, (void *) 0x11111111);
  EXPECT_EQ(list->cap, 10);
  EXPECT_EQ(list->len, 10);
  EXPECT_TRUE(list->data);
  // expect it to be there
  EXPECT_EQ(list->data[5], (void *) 0x11111111);
  // expect the others to still be there
  EXPECT_EQ(list->data[0], (void *) 0xdeadbeef);
  EXPECT_EQ(list->data[9], (void *) 0xcafef00d);

  // cleanup
  alst_free(list, NULL);
}

TEST(ArrayListTest, Get) {
  alst_t *list = alst_make(10);

  // add one member, verify that alst_get gets it
  list->len = 1;
  alst_put(list, 0, (void *) 0xdeadbeef);
  void *val = alst_get(list, 0);
  EXPECT_EQ(val, (void *) 0xdeadbeef);

  // add a member to the end, verify that alst_get gets it (glassbox)
  list->len = 5;
  alst_put(list, 4, (void *) 0xcafef00d);
  val = alst_get(list, 4);
  EXPECT_EQ(val, (void *) 0xcafef00d);
  // expect the first member to still be there
  val = alst_get(list, 0);
  EXPECT_EQ(val, (void *) 0xdeadbeef);

  // cleanup
  alst_free(list, NULL);
}

TEST(ArrayListTest, Append) {
  alst_t *list = alst_make(10);

  // append 10 members
  for (size_t i = 0; i < list->cap; ++i) {
    alst_append(list, (void *) 0xdeadbeef);
  }
  
  // verify it's not broken
  EXPECT_EQ(list->len, 10);
  EXPECT_EQ(list->cap, 10);
  EXPECT_TRUE(list->data);
  
  // verify members are all there
  int match = 1;
  for (size_t i = 0; i < list->len; ++i) {
    match &= (list->data[i] == (void *) 0xdeadbeef);
  }
  EXPECT_TRUE(match);

  // append 11th member, expect capacity expansion
  alst_append(list, (void *) 0xcafef00d);
  EXPECT_EQ(list->len, 11);
  EXPECT_EQ(list->cap, 100);
  EXPECT_TRUE(list->data);

  // verify all 11 members are still there
  match = 1;
  for (size_t i = 0; i < 10; ++i) {
    match &= list->data[i] == (void *) 0xdeadbeef;
  }
  EXPECT_TRUE(match);
  EXPECT_EQ(list->data[10], (void *) 0xcafef00d);

  // cleanup
  alst_free(list, NULL);
}
