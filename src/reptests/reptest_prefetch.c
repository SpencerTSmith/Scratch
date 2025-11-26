#define LOG_TITLE "REPETITION_TESTER"
#define COMMON_IMPLEMENTATION
#include "../common.h"

#include "../benchmark/benchmark_inc.h"
#include "../benchmark/benchmark_inc.c"

typedef void Assmebly_Function(u8 *data, u64 *offsets, u64 offsets_count, u64 fake_op_count);

typedef struct Assmebly_Entry Assembly_Entry;
struct Assmebly_Entry
{
  String name;
  Assmebly_Function *function;
};

extern void read_no_fetch_asm(u8 *data, u64 *offsets, u64 offsets_count, u64 fake_op_count);
extern void read_prefetch_asm(u8 *data, u64 *offsets, u64 offsets_count, u64 fake_op_count);

Assembly_Entry test_entries[] =
{
  {STR("read_no_fetch_asm"), read_no_fetch_asm},
  {STR("read_prefetch_asm"), read_prefetch_asm},
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

  u64 offsets[KB(512)] = {0};

  u64 total_size_processed = STATIC_COUNT(offsets) * cacheline_size;

  u64 fake_op_counts[16] = {0};

  Repetition_Tester testers[STATIC_COUNT(fake_op_counts)][STATIC_COUNT(test_entries)] = {0};

  for (usize fake_op_idx = 0; fake_op_idx < STATIC_COUNT(fake_op_counts); fake_op_idx++)
  {
    u64 *fake_op_count = fake_op_counts + fake_op_idx;

    *fake_op_count = 4 * (fake_op_idx + 1);

    for (usize func_idx = 0; func_idx < STATIC_COUNT(test_entries); func_idx++)
    {
      // New buffer
      u64 data_count = GB(1);
      u8 *data = os_allocate(data_count, OS_ALLOCATION_COMMIT|OS_ALLOCATION_PREFAULT);

      u64 line_count = data_count / cacheline_size;

      // Random offsets
      for (usize offset_idx = 0; offset_idx < STATIC_COUNT(offsets); offset_idx++)
      {
        u64 *offset = offsets + offset_idx;

        *offset = (rand() % (line_count)) * cacheline_size;
      }

      Repetition_Tester *tester = &testers[fake_op_idx][func_idx];

      Assembly_Entry *entry = test_entries + func_idx;

      printf("\n--- %.*s (%lu fake ops) ---\n", STRF(entry->name), *fake_op_count);

      repetition_tester_new_wave(tester, total_size_processed, cpu_timer_frequency, seconds_to_try_for_min);
      while (repetition_tester_is_testing(tester))
      {
        repetition_tester_begin_time(tester);
        entry->function(data, offsets, STATIC_COUNT(offsets), *fake_op_count);
        repetition_tester_close_time(tester);

        repetition_tester_count_bytes(tester, total_size_processed);
      }

      os_deallocate(data, data_count);
    }
  }

  // Dump csv
  printf("Function");
  for (usize fake_op_idx = 0; fake_op_idx < STATIC_COUNT(fake_op_counts); fake_op_idx++)
  {
    printf(",%lu", fake_op_counts[fake_op_idx]);
  }
  printf("\n");

  for (usize func_idx = 0; func_idx < STATIC_COUNT(test_entries); func_idx++)
  {
    printf("%.*s", STRF(test_entries[func_idx].name));
    for (usize fake_op_idx = 0; fake_op_idx < STATIC_COUNT(fake_op_counts); fake_op_idx++)
    {
      Repetition_Test_Values min = testers[fake_op_idx][func_idx].results.min;

      f64 seconds = cpu_time_in_seconds(min.v[REPTEST_VALUE_TIME], cpu_timer_frequency);
      f64 gb_per_s = min.v[REPTEST_VALUE_BYTE_COUNT]  / (f64)GB(1) / seconds;

      printf(",%.4f", gb_per_s);
    }
    printf("\n");
  }
}
