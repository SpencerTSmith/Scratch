#define LOG_TITLE "REPETITION_TESTER"
#define COMMON_IMPLEMENTATION
#include "../common.h"

#include "../benchmark/benchmark_inc.h"
#include "../benchmark/benchmark_inc.c"

typedef void Assmebly_Function(u64 count, u64 *data);

typedef struct Assmebly_Entry Assembly_Entry;
struct Assmebly_Entry
{
  String name;
  Assmebly_Function *function;
};

extern void read32_asm(u64 count, u64 *data);
extern void read64_asm(u64 count, u64 *data);
extern void read128_asm(u64 count, u64 *data);
extern void read256_asm(u64 count, u64 *data);
// extern void read512_asm(u64 count, u64 *data);

Assembly_Entry test_entries[] =
{
  {STR("read32_asm"), read32_asm},
  {STR("read64_asm"), read64_asm},
  {STR("read128_asm"), read128_asm},
  {STR("read256_asm"), read256_asm},
};

int main(int arg_count, char **args)
{
  if (arg_count != 2)
  {
    printf("Usage: %s [seconds_to_try_for_min]\n", args[0]);
  }

  u64 count = 1024 * 1024 * 1024;

  u64 cpu_timer_frequency = estimate_cpu_timer_freq();

  u32 seconds_to_try_for_min = atoi(args[1]);

  u64 data[256] = {10};

  while (true)
  {
    Repetition_Tester testers[STATIC_ARRAY_COUNT(test_entries)] = {0};

    for (usize func_idx = 0; func_idx < STATIC_ARRAY_COUNT(test_entries); func_idx++)
    {
      Repetition_Tester *tester = &testers[func_idx];

      Assembly_Entry *entry = &test_entries[func_idx];

      printf("\n--- %.*s ---\n", String_Format(entry->name));
      printf("                                                          \r");

      repetition_tester_new_wave(tester, count, cpu_timer_frequency, seconds_to_try_for_min);
      while (repetition_tester_is_testing(tester))
      {
        repetition_tester_begin_time(tester);
        entry->function(count, data);
        repetition_tester_close_time(tester);

        repetition_tester_count_bytes(tester, count);
      }
    }
  }
}
