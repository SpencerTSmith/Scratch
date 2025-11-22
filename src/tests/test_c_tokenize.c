#define COMMON_IMPLEMENTATION
#include "../common.h"

#include "../c_tokenize.c"

int main(int argc, char **argv)
{
  Arena arena = arena_make();
  {
    String sample_program =
      STR(
        "typedef struct Foo_s\n"
        "{\n"
        "  int boo;\n"
        "  char bar;\n"
        "} Foo;\n"
        "};\n"
        "typedef enum Baz_e\n"
        "{\n"
        "  BAZ_BOO,\n"
        "  BAZ_BAR,\n"
        "  BAZ_BAR2,\n"
        "} Baz;\n"
        "};\n"
        "int main(int argc, char **argv)\n"
        "{\n"
        "  // I'm a comment!\n"
        "  /* I'm a comment, too! */\n"
        "  long l = 2L;\n"
        "  long ull = 10000ulL;\n"
        "  float f = 0.11f;\n"
        "  double d = 0.111;\n"
        "  int intensity = 0x1000;\n"
        "  int i = 1;\n"
        "  int j = i++;\n"
        "  if (i == j)\n"
        "  {\n"
        "    j = -1;\n"
        "    i--;\n"
        "  }\n"
        "  if (i && j)\n"
        "  {\n"
        "    j = j & i;\n"
        "    i &= 1;\n"
        "  }\n"
        "  j = i * j;\n"
        "  j = ~j;\n"
        "  char c = '0';\n"
        "  char escape = '\\'';\n"
        "  printf(\"Hello, \\\"world!\\\"\\n\");\n"
        "  return 0;\n"
        "}\n"
      );

    C_Token_Array tokens = tokenize_c_code(&arena, sample_program);
    for EACH_INDEX(token_idx, tokens.count)
    {
      C_Token token = tokens.v[token_idx];
      printf("Token %lu: %s [ %.*s ]\n",
             token_idx, C_Token_Type_strings[token.type], STRF(token.raw));
    }
  }
}
