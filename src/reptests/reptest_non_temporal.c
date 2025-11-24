#define LOG_TITLE "REPETITION_TESTER"
#define COMMON_IMPLEMENTATION
#include "../common.h"

#include "../benchmark/benchmark_inc.h"
#include "../benchmark/benchmark_inc.c"

typedef void Assmebly_Function(u8 *out, u64 out_count, u8 *in, u64 in_count);

typedef struct Assmebly_Entry Assembly_Entry;
struct Assmebly_Entry
{
  String name;
  Assmebly_Function *function;
};

extern void read_temporal_asm(u8 *out, u64 out_count, u8 *in, u64 in_count);
extern void read_non_temporal_asm(u8 *out, u64 out_count, u8 *in, u64 in_count);

Assembly_Entry test_entries[] =
{
  {STR("read_temporal_asm"),     read_temporal_asm},
  {STR("read_non_temporal_asm"), read_non_temporal_asm},
};

int main(int arg_count, char **args)
{
  if (arg_count != 2)
  {
    printf("Usage: %s [seconds_to_try_for_min]\n", args[0]);
    return -1;
  }

  u64 cpu_timer_frequency = estimate_cpu_timer_freq();

  u32 seconds_to_try_for_min = atoi(args[1]);

  u64 cacheline_size = 64;

  u64 in_count = cacheline_size * 256;
  u8 *in = os_allocate(in_count, OS_ALLOCATION_COMMIT|OS_ALLOCATION_PREFAULT);

  u64 out_count = in_count * 1024 * 64;
  u8 *out = os_allocate(out_count, OS_ALLOCATION_COMMIT|OS_ALLOCATION_PREFAULT);

  Repetition_Tester testers[STATIC_COUNT(test_entries)] = {0};

  for (usize func_dix = 0; func_dix < STATIC_COUNT(testers); func_dix++)
  {
    Repetition_Tester *tester = testers + func_dix;

    Assembly_Entry *entry = test_entries + func_dix;

    printf("\n--- %.*s Reading in %lu bytes, writing to %lu bytes ---\n",
           STRF(entry->name), in_count, out_count);

    repetition_tester_new_wave(tester, out_count, cpu_timer_frequency, seconds_to_try_for_min);
    while (repetition_tester_is_testing(tester))
    {
      repetition_tester_begin_time(tester);
      entry->function(out, out_count, in, in_count);
      repetition_tester_close_time(tester);

      repetition_tester_count_bytes(tester, out_count);
    }
  }

  os_deallocate(in, in_count);
  os_deallocate(out, out_count);
}
