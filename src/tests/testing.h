#ifndef TESTING_H
#define TESTING_H
#include "../common.h"

// NOTE: Not a good idea to nest blocks

typedef struct Tester_Block Tester_Block;
struct Tester_Block
{
  String label;
  usize  pass_count;
  usize  fail_count;
  usize  total_count;
};

typedef struct Tester Tester;
struct Tester
{
  Tester_Block blocks[4096];
  usize block_count;
  usize total_pass_count;
  usize total_fail_count;
  usize total_count;
};

static
Tester g_tester = {0};

static
void tester_summarize();

static
Tester_Block *__test_begin_block(String label);

static
void __test_close_block(Tester_Block *block);

#define TEST_BLOCK(label) \
  for (Tester_Block *_test_block_ = __test_begin_block((label)); _test_block_->total_count == 0; __test_close_block(_test_block_))

#define TEST_EVAL(expr)                                                                                       \
  STATEMENT                                                                                                   \
  (                                                                                                           \
    b32 __expr_eval = (expr);                                                                                 \
    const char *message = ANSI_GREEN "PASS :)" ANSI_RESET;                                                    \
    if (__expr_eval)                                                                                          \
    {                                                                                                         \
      _test_block_->pass_count += 1;                                                                          \
    }                                                                                                         \
    else                                                                                                      \
    {                                                                                                         \
      _test_block_->fail_count += 1;                                                                          \
      message =  ANSI_RED "FAIL :( @" __FILE__":" STRINGIFY(__LINE__) ANSI_RESET;                             \
    }                                                                                                         \
    _test_block_->total_count += 1;                                                                           \
                                                                                                              \
    printf("  [%s]: %s\n", #expr, message);                                                                   \
                                                                                                              \
  )

#define EPSILON_EQUAL(a, b) (fabs((a) - (b)) < 0.0001f)
#define VEC2_EPSILON_EQUAL(v1, v2) (EPSILON_EQUAL((v1).x, (v2).x) && EPSILON_EQUAL((v1).y, (v2).y))
#define VEC3_EPSILON_EQUAL(v1, v2) (EPSILON_EQUAL((v1).x, (v2).x) && EPSILON_EQUAL((v1).y, (v2).y) && EPSILON_EQUAL((v1).z, (v2).z))
#define VEC4_EPSILON_EQUAL(v1, v2) (EPSILON_EQUAL((v1).x, (v2).x) && EPSILON_EQUAL((v1).y, (v2).y) && EPSILON_EQUAL((v1).z, (v2).z) && EPSILON_EQUAL((v1).w, (v2).w))

#endif // TESTING_H
