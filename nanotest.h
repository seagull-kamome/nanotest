/*! \file
 *
 * \verbatim
 * Copyright 2017 (C) seagull all rights reserved.
 * This program licenseed under BSD3.
 * \endvarbatim
 */
#if !defined(__NANOTEST_H__)
#define __NANOTEST_H__

#define USE_NANOTEST 1

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/*! \addtogroup LogOutput Log output
 * @{ */

/*! Put isngle char to log.
 * You MUST provide this function for me
 */
void nanotest_putchar(char ch);


/*! Put newline to log */
void nanotest_nl();
void nanotest_puthline(char ch, unsigned int n);
void nanotest_putstr(char const* str);
void nanotest_putstrnl(char const* str);
void nanotest_putuint(unsigned int n);
void nanotest_putuint_hex(unsigned int n);
void nanotest_putfileline(
        char const* fname, unsigned int line, char const* desc);
/* @} */


extern char const nanotest_str_false[];
extern char const nanotest_str_true[];
extern char const nanotest_str_equal[];
extern char const nanotest_str_notequal[];

struct nanotest_testcase {
  char const* name;
  void (*startup)();
  void (*teardown)();
  void (*startup_test)();
  void (*teardown_test)();
};

#define NANOTEST_TEST_FLAGS_RUNNED      (0x01)
#define NANOTEST_TEST_FLAGS_HAS_ERROR   (0x02)
struct nanotest_test {
  char const* name;
  struct nanotest_testcase const* testcase;
  void (*run)();
  unsigned int* flags;
};

#define NANOTEST_MAX_TESTS    (200)
void nanotest_register_test(struct nanotest_test const* tst);
void nanotest_runtest(char const* targetcase, char const* targettest);
#define DEFAULT_MAIN(f) \
  void nanotest_putchar(char ch) { (f)(ch); } \
  int main() { nanotest_runtest(NULL, NULL); }


void nanotest_failed_uint(
        unsigned int expected,
        unsigned int actual,
        char const* opr,
        char const* desc,
        char const* fname,
        unsigned int lineno);


void nanotest_failed_bool(
        bool expected,
        char const* desc,
        char const* fname,
        unsigned int lineno);


#define TESTCASE(c) \
  void testcase_ ## c ## _startup() {} \
  void testcase_ ## c ## _teardown() {} \
  void testcase_ ## c ## _startup_test() {} \
  void testcase_ ## c ## _teardown_test() {} \
  struct nanotest_testcase const testcase_ ## c ## _info = { \
    (#c), \
    (testcase_ ## c ## _startup), (testcase_ ## c ## _teardown), \
    (testcase_ ## c ## _startup_test), (testcase_ ## c ## _teardown_test), \
  };


#define TEST(c, t) \
  unsigned int testcase_ ## c ## _test_ ## t ## _flag; \
  void  testcase_ ## c ## _test_ ## t ## _run(); \
  struct nanotest_test const testcase_ ## c ## _test_ ## t ## _info = { \
    (#t), &(testcase_ ## c ## _info), \
    (testcase_ ## c ## _test_ ## t ## _run), \
    &(testcase_ ## c ## _test_ ## t ## _flag) \
  }; \
  void __attribute__((constructor)) testcase_ ## c ## _test_ ## t ## _register() { \
    nanotest_register_test(&(testcase_ ## c ## _test_ ## t ## _info)); } \
  void  testcase_ ## c ## _test_ ## t ## _run()


#define ASSERT_EQ(e, a) \
  do { \
    __typeof(a) x = (a); \
    if (! (((__typeof(a))(e)) == x)) { \
        nanotest_failed_uint((e), x, nanotest_str_equal, NULL, __FILE__, __LINE__); \
    } } while (0)

#define ASSERT_NE(e, a) \
  do { \
    __typeof(a) x = (a); \
    if (! (((__typeof(a))(e)) != x)) { \
      nanotest_failed_uint((e), x, nanotest_str_notequal, NULL, __FILE__, __LINE__); \
    } } while (0)

#define ASSERT_TRUE(a) \
  do { \
    bool x = (a); \
    if (! (x) ){ \
      nanotest_failed_bool(true, NULL, __FILE__, __LINE__); \
    } } while (0)
#define ASSERT_FALSE(a) \
  do { \
    bool x = (a); \
    if (!! (x)) { \
      nanotest_failed_bool(false, NULL, __FILE__, __LINE__); \
    } } while (0)



/*! Mock for IO registers */
#define NANOTEST_NUM_IOREGION   20
void nanotest_init_ioregion();
void nanotest_add_32bit_ioregion(
        uintptr_t startaddr,
        size_t len,
        uint32_t* buf);

uint32_t nanotest_read_reg32(
        uintptr_t ioaddr,
        char const* fname, unsigned int lineno);
void nanotest_write_reg32(
        uintptr_t ioaddr,
        uint32_t val,
        char const* fname, unsigned int lineno);
void nanotest_write_with_mask_reg32(
        uintptr_t ioaddr,
        uint32_t msk, uint32_t val,
        char const* fname, unsigned int lineno);

#endif
// vim : sw=8 ts=4 tw=0 expandtab cindent
