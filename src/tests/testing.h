#include "../common.h"

// NOTE: Probably not a good idea to nest blocks

typedef struct Tester_Block Tester_Block;
struct Tester_Block
{
  String label;
  u64    pass_count;
  u64    fail_count;
  u64    total_count;
};

typedef struct Tester Tester;
struct Tester
{
  Tester_Block blocks[4096];
  usize current_block;
  u64   total_pass_count;
  u64   total_fail_count;
  u64   total_count;
};

static Tester g_tester = {0};

static
Tester_Block __test_begin_block(String label, usize index);

static
void __test_close_block(Tester_Block block);

// HACK: Don't use with profiler! Both use __COUNTER__ !
#define test_begin_block(label) __test_begin_block((label), __COUNTER__ + 1)
#define test_close_block(block) __test_close_block((block))

#define TEST_BLOCK(label) \
  for (Tester_Block block = test_begin_block((label)); block.total_count == 0; test_close_block(block))
