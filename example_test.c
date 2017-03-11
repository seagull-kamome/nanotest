/*! \file
 */
#include "nanotest.h"
#include <stdio.h>

TESTCASE(assertion);

TEST(assertion, uint_eq) {
  ASSERT_EQ(0, 0);
  ASSERT_EQ(0, 1);
  ASSERT_EQ(0, 2);

  ASSERT_EQ(0xffffffff, 0);
  ASSERT_EQ(0xffffffff, 1);
  ASSERT_EQ(0xffffffff, 0xffffffff);
}

TEST(assertion, uint_ne) {
  ASSERT_NE(0, 0);
  ASSERT_NE(0, 1);
  ASSERT_NE(0, 2);

  ASSERT_NE(0xffffffff, 0);
  ASSERT_NE(0xffffffff, 1);
  ASSERT_NE(0xffffffff, 0xffffffff);
}


TEST(assertion, bool) {
  ASSERT_TRUE(true);
  ASSERT_TRUE(false);

  ASSERT_FALSE(true);
  ASSERT_FALSE(false);
}


TEST(assertion, success) {
  ASSERT_TRUE(true);
  ASSERT_FALSE(false);
  ASSERT_EQ(512, 512);
  ASSERT_NE(1024, 999);
}


DEFAULT_MAIN(putchar);

