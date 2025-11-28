#define LOG_TITLE "REPETITION_TESTER"
#define COMMON_IMPLEMENTATION
#include "../common.h"

#include "../benchmark/benchmark_inc.h"
#include "../benchmark/benchmark_inc.c"

#include <fcntl.h>
#include <unistd.h>

typedef struct Operation_Parameters Operation_Parameters;
struct Operation_Parameters
{
  const char *file_name;
  u64 file_size;
  u64 chunk_size;
};

static
void read_baseline(Repetition_Tester *tester, Operation_Parameters *params)
{
  u8 *buffer = os_allocate(params->file_size, OS_ALLOCATION_COMMIT);

  FILE *file = fopen(params->file_name, "rb");
  if (buffer && file)
  {
    repetition_tester_begin_time(tester);

    u64 result = fread(buffer, params->file_size, 1, file);

    repetition_tester_close_time(tester);

    if (result == 1)
    {
      repetition_tester_count_bytes(tester, params->file_size);
    }
    else
    {
      repetition_tester_error(tester, "Unable to read file");
    }

    fclose(file);
  }

  os_deallocate(buffer, params->file_size);
}

static
void read_chunk_crt_fread(Repetition_Tester *tester, Operation_Parameters *params)
{
  u8 *buffer = os_allocate(params->chunk_size, OS_ALLOCATION_COMMIT);

  FILE *file = fopen(params->file_name, "rb");
  if (buffer && file)
  {
    repetition_tester_begin_time(tester);

    u64 remaining = params->file_size;
    while (remaining)
    {
      u64 read_size = params->chunk_size > remaining ? remaining : params->chunk_size;

      u64 result = fread(buffer, read_size, 1, file);

      if (result == 1)
      {
        repetition_tester_count_bytes(tester, read_size);
      }
      else
      {
        repetition_tester_error(tester, "Unable to read file");
      }

      remaining -= read_size;
    }

    repetition_tester_close_time(tester);

    fclose(file);
  }
  else
  {
    repetition_tester_error(tester, "Unable to open file");
  }

  os_deallocate(buffer, params->chunk_size);
}

static
void read_chunk_unix_read(Repetition_Tester *tester, Operation_Parameters *params)
{
  u8 *buffer = os_allocate(params->chunk_size, OS_ALLOCATION_COMMIT); // No prefault

  int fd = open(params->file_name, O_RDONLY);

  if (buffer && fd != -1)
  {
    repetition_tester_begin_time(tester);

    u64 remaining = params->file_size;
    while (remaining)
    {
      u64 read_size = params->chunk_size > remaining ? remaining : params->chunk_size;

      u64 result = read(fd, buffer, read_size);

      if (result == read_size)
      {
        repetition_tester_count_bytes(tester, read_size);
      }
      else
      {
        repetition_tester_error(tester, "Unable to read file");
      }

      remaining -= read_size;
    }

    repetition_tester_close_time(tester);

    close(fd);
  }
  else
  {
    repetition_tester_error(tester, "Unable to open file");
  }

  os_deallocate(buffer, params->chunk_size);
}

Operation_Entry test_entries[] =
{
  {STR("no chunk read baseline"), read_baseline},
  {STR("chunk crt fread"), read_chunk_crt_fread},
  {STR("chunk unix read"), read_chunk_unix_read},
};

int main(int arg_count, char **args)
{
  if (arg_count != 3)
  {
    printf("Usage: %s [test_file] [seconds_to_try_for_min]\n", args[0]);
    return 1;
  }

  u64 size = file_size(args[1]);

  u64 cpu_timer_frequency = estimate_cpu_timer_freq();

  u32 seconds_to_try_for_min = atoi(args[2]);

  Repetition_Tester testers[1024][STATIC_ARRAY_COUNT(test_entries)] = {0};

  for (usize chunk_size = KB(64), chunk_idx = 0; chunk_size <= size && chunk_idx < 1024; chunk_size *= 2, chunk_idx++)
  {
    Operation_Parameters params =
    {
      .file_size = size,
      .file_name = args[1],
      .chunk_size = chunk_size,
    };

    for (usize func_idx = 0; func_idx < STATIC_ARRAY_COUNT(test_entries); func_idx++)
    {
      Repetition_Tester *tester = &testers[chunk_idx][func_idx];
      Operation_Entry entry = test_entries[func_idx];

      printf("\n--- %.*s (%lu kb chunk) ---\n", STRF(entry.name), chunk_size / KB(1));
      repetition_tester_new_wave(tester, size, cpu_timer_frequency, seconds_to_try_for_min);
      while (repetition_tester_is_testing(tester))
      {
        entry.function(tester, &params);
      }
    }
  }

  printf("chunk_size");
  for (usize func_idx = 0; func_idx < STATIC_ARRAY_COUNT(test_entries); func_idx++)
  {
    Operation_Entry entry = test_entries[func_idx];
    printf(",%.*s", STRF(entry.name));
  }
  printf("\n");

  for (usize chunk_size = KB(64), chunk_idx = 0; chunk_size <= size && chunk_idx < 1024; chunk_size *= 2, chunk_idx++)
  {
    printf("%lu", chunk_size);
    for (usize func_idx = 0; func_idx < STATIC_ARRAY_COUNT(test_entries); func_idx++)
    {
      Repetition_Test_Values min = testers[chunk_idx][func_idx].results.min;

      f64 seconds = cpu_time_in_seconds(min.v[REPTEST_VALUE_TIME], cpu_timer_frequency);
      f64 gb_per_s = min.v[REPTEST_VALUE_BYTE_COUNT]  / (f64)GB(1) / seconds;

      printf(",%.4f", gb_per_s);
    }

    printf("\n");
  }
}
