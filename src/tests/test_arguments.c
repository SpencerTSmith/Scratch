#define COMMON_IMPLEMENTATION
#include "../common.h"

#include "testing.h"
#include "testing.c"

int main(int argc, char **argv)
{
  Arena arena = arena_make();
  Args arguments = parse_args(&arena, argc, argv);

  // printf("\nVisual Check -----\n");
  // {
  //   printf("Program Name: %.*s\n", String_Format(arguments.program_name));
  //
  //   for (usize i = 0; i < arguments.positionals_count; i++)
  //   {
  //     printf("Positional %td: %.*s\n", i, String_Format(arguments.positionals[i]));
  //   }
  //
  //   for (usize slot = 0; slot < arguments.option_table_count; slot++)
  //   {
  //     Arg_Option *first = arguments.option_table + slot;
  //
  //     // Filled bucket
  //     if (first->name.v)
  //     {
  //       for (Arg_Option *node = first; node; node = node->hash_next)
  //       {
  //         printf("Option: %.*s\n", String_Format(node->name));
  //         for (usize i = 0; i < node->values.count; i++)
  //         {
  //           printf("  Value: %.*s\n", String_Format(node->values.v[i]));
  //         }
  //       }
  //     }
  //   }
  // }

  TEST_BLOCK(STR("args_has_flag"))
  {
    TEST_EVAL(args_has_flag(&arguments, String("foo")));
  }

  TEST_BLOCK(STR("args_get_option_values"))
  {
    String_Array values = args_get_option_values(&arguments, String("baz"));
    TEST_EVAL(string_match(values.v[0], String("foo")));
    TEST_EVAL(string_match(values.v[1], String("bar")));
    TEST_EVAL(string_match(values.v[2], String("boo")));
  }

  TEST_BLOCK(STR("args_get_option_values"))
  {
    String_Array values = args_get_option_values(&arguments, String("bunk"));
    TEST_EVAL(string_match(values.v[0], String("bip")));
    TEST_EVAL(string_match(values.v[1], String("bop")));
    TEST_EVAL(string_match(values.v[2], String("bam")));
  }

  TEST_BLOCK(STR("args_positionals"))
  {
    String *positionals = arguments.positionals;
    TEST_EVAL(arguments.positionals_count == 2);
    TEST_EVAL(string_match(positionals[0], String("positional")));
    TEST_EVAL(string_match(positionals[1], String("positional2")));
  }

  tester_summarize();

  arena_free(&arena);
}
