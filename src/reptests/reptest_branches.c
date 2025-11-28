#define LOG_TITLE "REPETITION_TESTER"
#define COMMON_IMPLEMENTATION
#include "../common.h"

#include <time.h>

#include "../benchmark/benchmark_inc.h"
#include "../benchmark/benchmark_inc.c"

typedef enum Branch_Pattern
{
  BRANCH_NEVER,

  BRANCH_ALWAYS,
  BRANCH_EVERY_TWO,
  BRANCH_EVERY_THREE,
  BRANCH_EVERY_FOUR,
  BRANCH_EVERY_FIVE,
  BRANCH_CRT_RANDOM,
  BRANCH_OS_RANDOM,

  BRANCH_COUNT,
} Branch_Pattern;

const char *branch_pattern_strings[BRANCH_COUNT] =
{
  [BRANCH_NEVER]       = "never",
  [BRANCH_ALWAYS]      = "always",
  [BRANCH_EVERY_TWO]   = "every-two",
  [BRANCH_EVERY_THREE] = "every-three",
  [BRANCH_EVERY_FOUR]  = "every-four",
  [BRANCH_EVERY_FIVE]  = "every-five",
  [BRANCH_CRT_RANDOM]  = "crt-random",
  [BRANCH_OS_RANDOM]   = "os-random",
};

void fill_buffer_branch_pattern(String buffer, Branch_Pattern pattern)
{
  if (pattern == BRANCH_OS_RANDOM)
  {
    if (!os_get_random_bytes(buffer.v, buffer.count))
    {
      LOG_ERROR("Uh oh!");
    }
  }
  else
  {
    for (usize i = 0; i < buffer.count; i++)
    {
      u8 value = 0;
      switch (pattern)
      {
        case BRANCH_NEVER:
        {
          value = 0;
        }
        break;
        case BRANCH_ALWAYS:
        {
          value = 1;
        }
        break;
        case BRANCH_EVERY_TWO:
        {
          value = (i % 2 == 0);
        }
        break;
        case BRANCH_EVERY_THREE:
        {
          value = (i % 3 == 0);
        }
        break;
        case BRANCH_EVERY_FOUR:
        {
          value = (i % 4 == 0);
        }
        break;
        case BRANCH_EVERY_FIVE:
        {
          value = (i % 5 == 0);
        }
        break;
        case BRANCH_CRT_RANDOM:
        {
          value = (u8)rand();
        }
        break;

        default:
        {
          LOG_ERROR("Invalid branch pattern");
        }
        break;
      }

      buffer.v[i] = value;
    }
  }
}

typedef struct Operation_Parameters Operation_Parameters;
struct Operation_Parameters
{
  String buffer;
};

extern void branch_bytes_asm(u64 count, u8 *data);

static
void branch_bytes(Repetition_Tester *tester, Operation_Parameters *params)
{
  String buffer = params->buffer;

  repetition_tester_begin_time(tester);
  branch_bytes_asm(buffer.count, buffer.v);
  repetition_tester_close_time(tester);

  repetition_tester_count_bytes(tester, buffer.count);
}

Operation_Entry test_entries[] =
{
  {String("Branch"),  branch_bytes},
};

int main(int arg_count, char **args)
{
  if (arg_count != 2)
  {
    printf("Usage: %s [seconds_to_try_for_min]\n", args[0]);
  }

  srand(time(NULL));

  usize size = GB(1) + 8;
  Operation_Parameters params =
  {
    .buffer =
    {
      .v  = os_allocate(size, OS_ALLOCATION_COMMIT),
      .count = size,
    },
  };

  u64 cpu_timer_frequency = estimate_cpu_timer_freq();

  u32 seconds_to_try_for_min = atoi(args[1]);

  while (true)
  {
    Repetition_Tester testers[STATIC_ARRAY_COUNT(test_entries)][BRANCH_COUNT] = {0};

    for (usize test_func_idx = 0; test_func_idx < STATIC_ARRAY_COUNT(test_entries); test_func_idx++)
    {
      Operation_Entry *entry = &test_entries[test_func_idx];

      for (usize pattern_idx = 0; pattern_idx < BRANCH_COUNT; pattern_idx++)
      {
        Repetition_Tester *tester = &testers[test_func_idx][pattern_idx];

        printf("\n--- %.*s %s ---\n", String_Format(entry->name), branch_pattern_strings[pattern_idx]);
        printf("                                                          \r");

        fill_buffer_branch_pattern(params.buffer, (Branch_Pattern)pattern_idx);

        repetition_tester_new_wave(tester, size, cpu_timer_frequency, seconds_to_try_for_min);
        while (repetition_tester_is_testing(tester))
        {
          entry->function(tester, &params);
        }
      }
    }
  }

  os_deallocate(params.buffer.v, params.buffer.count);
}
