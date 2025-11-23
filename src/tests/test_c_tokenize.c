#define COMMON_IMPLEMENTATION
#include "../common.h"

#include "../c_tokenize.c"

int main(int argc, char **argv)
{
  Arena arena = arena_make();
  {
    String sample_program =
      STR(
        "   `\n"
        " `\n"
        "typedef struct Foo_s\n"
        "{\n"
        "  int boo;\n"
        "  char bar;\n"
        "} Foo;\n"
        "};\n"
        "typedef enum Baz_e\n"
        "`\n"
        "{\n"
        "  BAZ_BOO,\n"
        "  BAZ_BAR,\n"
        "  BAZ_BAR2,\n"
        "} Baz;\n"
        "};\n"
        "int main(int argc, char **argv)\n"
        "{\n"
        "  // I'm a comment!\n"
        "`\n"
        "  /* I'm a comment, too! */\n"
        "  long l = 2L;\n"
        "  long ull = 10000ulL;\n"
        "  float f = 0.11f;\n"
        "  double d = 0.111;\n"
        "  double div = d / f;\n"
        "  int intensity = 0x1000;\n"
        "  int i = 1;\n"
        "  int j = i++;\n"
        "  if (i == j)\n"
        "  {\n"
        "    j = -1;\n"
        "    i--;\n"
        "  }\n"
        "  else if (i != j)\n"
        "  {\n"
        "    j = -1;\n"
        "    i--;\n"
        "  }\n"
        "  if (!i)\n"
        "  {\n"
        "  }\n"
        "  if (i && j)\n"
        "  {\n"
        "    j = j & i;\n"
        "    i &= 1;\n"
        "  }\n"
        "`\n"
        "  j = i * j;\n"
        "  j = ~j;\n"
        "  char c = '0';\n"
        "  char escape = '\\'';\n"
        "  printf(\"Hello, \\\"world!\\\"\\n\");\n"
        "  return 0;\n"
        "  /\n"
        "}\n"
        "`\n"
      );

    C_Token_Array tokens = tokenize_c_code(&arena, sample_program);
    // for EACH_INDEX(token_idx, tokens.count)
    // {
    //   C_Token token = tokens.v[token_idx];
    //   printf("Token %lu: %s [ %.*s ]\n",
    //          token_idx, C_Token_Type_strings[token.type], STRF(token.raw));
    // }
  }
  // {
  //   String taco = read_file_to_arena(&arena, STR("taco_test.txt"));
  //
  //   C_Token_Array tokens = tokenize_c_code(&arena, taco);
  //   for EACH_INDEX(token_idx, tokens.count)
  //   {
  //     C_Token token = tokens.v[token_idx];
  //     printf("Token %lu: %s [ %.*s ]\n",
  //            token_idx, C_Token_Type_strings[token.type], STRF(token.raw));
  //   }
  //
  //   for EACH_INDEX(token_idx, tokens.count)
  //   {
  //     C_Token token = tokens.v[token_idx];
  //     printf("%.*s ", STRF(token.raw));
  //     if (token.type == C_TOKEN_SEMICOLON || token.type == C_TOKEN_BEGIN_CURLY_BRACE || token.type == C_TOKEN_CLOSE_CURLY_BRACE)
  //     {
  //       printf("\n");
  //     }
  //   }
  //   printf("\n");
  // }
}
