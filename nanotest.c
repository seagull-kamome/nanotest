/*! \file
 *
 * \verbatim
 * Copyright 2017 (C) seagull all rights reserved.
 * This program licensed under BSD3.
 * \endvarbatim
 */

#include "nanotest.h"
#include <string.h>

void nanotest_puthline(char ch, unsigned int n) {
  nanotest_putchar('\n');
  while (n--) nanotest_putchar(ch);
  nanotest_putchar('\n');
}

void nanotest_putnl() { nanotest_putchar('\n'); }

void nanotest_putstr(char const* str) {
  while (*str) nanotest_putchar(*(str++));
}
void nanotest_putstrnl(char const* str) {
  nanotest_putstr(str);
  nanotest_putnl();
}

void nanotest_putuint(unsigned int n) {
  unsigned int scale = 1;
  while (n /scale >= 10) scale *= 10;
  do {
    nanotest_putchar(n / scale + '0');
    n %= scale;
    scale /= 10;
  } while (scale >= 1);
}

void nanotest_putuint_hex(unsigned int n) {
  static char const hexes[] = "0123456789ABCDEF";
  nanotest_putstr("0x");
  for (int i = 8; i != 0; i--) {
    nanotest_putchar(hexes[n >> 28 & 0x0f]);
    n <<= 4;
  }
}

void nanotest_putfileline(
        char const* fname,
        unsigned int lineno,
        char const* desc) {
  nanotest_putstr(fname);
  nanotest_putstr(" (");
  nanotest_putuint(lineno);
  nanotest_putstr(") : ");
  if (desc != NULL)
    nanotest_putstr(desc);
  nanotest_putnl();
}


char const nanotest_str_false[] = "false";
char const nanotest_str_true[] = "true";
char const nanotest_str_equal[]    = "equal";
char const nanotest_str_notequal[] = "not equal";


static struct nanotest_test const* nanotest_all_tests[NANOTEST_MAX_TESTS];
static struct nanotest_test const** nanotest_last_test = nanotest_all_tests;
static struct nanotest_test const* nanotest_current_test = NULL;

void nanotest_register_test(struct nanotest_test const* tst) {
  if (nanotest_all_tests < nanotest_all_tests + NANOTEST_MAX_TESTS)
    *(nanotest_last_test++) = tst;
}


void nanotest_failed() {
  if (nanotest_current_test != NULL) {
    if (! (*(nanotest_current_test->flags) & NANOTEST_TEST_FLAGS_HAS_ERROR))
      nanotest_putstrnl("** FAILED **");
    *(nanotest_current_test->flags) |= NANOTEST_TEST_FLAGS_HAS_ERROR;
  }
}

void nanotest_failed_uint(
        unsigned int expected,
        unsigned int actual,
        char const* opr,
        char const* desc,
        char const* fname,
        unsigned int lineno) {
  nanotest_failed();

  nanotest_putstr("  - ");
  nanotest_putfileline(fname, lineno, desc);
  nanotest_putstr("      Expected : ");
  nanotest_putuint(expected);
  nanotest_putstr(" (");
  nanotest_putuint_hex(expected);
  nanotest_putstr(") ");
  nanotest_putstr(opr);
  nanotest_putstr("\n        Actual : ");
  nanotest_putuint(actual);
  nanotest_putstr(" (");
  nanotest_putuint_hex(actual);
  nanotest_putstrnl(") ");
}

void nanotest_failed_bool(
        bool expected,
        char const* desc,
        char const* fname,
        unsigned int lineno) {
  nanotest_failed();

  nanotest_putstr("  - ");
  nanotest_putfileline(fname, lineno, desc);
  nanotest_putstr("      Expected ");
  nanotest_putstr(expected? nanotest_str_true : nanotest_str_false);
  nanotest_putstrnl(" but actually not.");
}


void nanotest_runtest(char const* targetcase, char const* targettest) {

  nanotest_putstr("Starting unit test");
  nanotest_puthline('=', 79);

  if (nanotest_last_test >= nanotest_all_tests + NANOTEST_MAX_TESTS) {
    nanotest_putstrnl("** WARNNING : Too meny tests registerd. "
                      "Some tests will be lack out");
  }

  unsigned int num_total_testcase = 0;
  unsigned int num_total_tests = 0;
  unsigned int num_total_fails = 0;

  for (;;) {
    // Find next testcase
    struct nanotest_testcase const* tc = NULL;
    for (struct nanotest_test const** p = nanotest_all_tests;
          p != nanotest_last_test; p++)
      if (! (*(*p)->flags & NANOTEST_TEST_FLAGS_RUNNED) &&
          (tc == NULL || strcmp(tc->name, (*p)->testcase->name) < 0))
        tc = (*p)->testcase;
    if (tc == NULL) break; // All tests completed.


    ++num_total_testcase;
    unsigned int num_tests = 0;
    unsigned int num_fails = 0;
    nanotest_putstr("\nRunning test case : ");
    nanotest_putstr(tc->name);
    nanotest_puthline('-', 60);

    if (tc->startup) tc->startup();

    for (struct nanotest_test const** p = nanotest_all_tests;
          p != nanotest_last_test; p++) {
      if ((*p)->testcase != tc || *(*p)->flags & NANOTEST_TEST_FLAGS_RUNNED)
        continue;

      nanotest_current_test = *p;
      nanotest_putstr("Testing ");
      nanotest_putstr((*p)->name);
      nanotest_putstr(" ... ");
      if (tc->startup_test) tc->startup_test();

      ++num_tests;
      ++num_total_tests;
      *(*p)->flags |= NANOTEST_TEST_FLAGS_RUNNED;
      (*p)->run();
      if (*(*p)->flags & NANOTEST_TEST_FLAGS_HAS_ERROR) {
        ++num_fails;
        ++num_total_fails;
      }

      if (tc->teardown_test) tc->teardown_test();
      if (! (*(*p)->flags & NANOTEST_TEST_FLAGS_HAS_ERROR))
        nanotest_putstrnl("done.");
    }

    if (tc->teardown) tc->teardown();
    nanotest_puthline('-', 60);
    nanotest_putstr("  ");
    nanotest_putuint(num_fails);
    nanotest_putstr(" fails in ");
    nanotest_putuint(num_tests);
    nanotest_putstrnl(" tests.");
  }

  nanotest_puthline('=', 79);
  nanotest_putstr("  ");
  nanotest_putuint(num_total_tests);
  nanotest_putstr(" tests completed in ");
  nanotest_putuint(num_total_testcase);
  nanotest_putstr(" cases with ");
  nanotest_putuint(num_total_fails);
  nanotest_putstrnl(" fails.");
}

// vim : sw=8 ts=4 tw=0 expandtab cindent
