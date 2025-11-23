#include "testing.h"

static
Tester_Block *__test_begin_block(String label)
{
  Tester_Block *block = g_tester.blocks + g_tester.block_count;
  block->label = label;

  printf("[%.*s]--------------\n", STRF(label));

  g_tester.block_count += 1;

  return block;
}

static
void __test_close_block(Tester_Block *block)
{
  printf("--------------------\n\n");
  g_tester.total_pass_count += block->pass_count;
  g_tester.total_fail_count += block->fail_count;
  g_tester.total_count += block->total_count;
}

static
void tester_summarize()
{
  printf("SUMMARY:\n");
  printf("  Total: %lu\n",      g_tester.total_count);
  printf("  Total Pass:" ANSI_GREEN " %lu\n" ANSI_RESET, g_tester.total_pass_count);
  printf("  Total Fail:" ANSI_RED   " %lu\n" ANSI_RESET, g_tester.total_fail_count);
  printf("\n");

  if (g_tester.total_fail_count)
  {
    for EACH_INDEX(block_idx, g_tester.block_count)
    {
      Tester_Block block = g_tester.blocks[block_idx];
      if (block.fail_count)
      {
        printf("  [%.*s]--------------\n", STRF(block.label));
        printf("  | Total: %lu\n",      block.total_count);
        printf("  | Total Pass:" ANSI_GREEN " %lu\n" ANSI_RESET, block.pass_count);
        printf("  | Total Fail:" ANSI_RED   " %lu\n" ANSI_RESET, block.fail_count);
        printf("  --------------------\n\n");
      }
    }
  }
}
