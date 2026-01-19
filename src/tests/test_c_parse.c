#define COMMON_IMPLEMENTATION
#include "../common.h"

#include "testing.h"
#include "testing.c"

#include "../c_tokenize.c"
#include "../c_parse.c"

void print_c_ast(C_Node *node, usize depth)
{
  if (node)
  {
    for (usize i = 0; i < depth; i++)
    {
      printf(" ");
    }

    printf("%s\n", C_Node_Type_strings[node->type]);

    for (C_Node *cursor = node->first_child; cursor; cursor = cursor->next_sibling)
    {
      print_c_ast(cursor, depth + 1);
    }
  }
}

int main(int argc, char **argv)
{
  Arena arena = arena_make();
  {
    String sample_program =
      STR(
        "int foo;"
      );

    C_Token_Array tokens = tokenize_c_code(&arena, sample_program);
    for EACH_INDEX(token_idx, tokens.count)
    {
      C_Token token = tokens.v[token_idx];
      printf("Token %lu: %s [ %.*s ]\n",
             token_idx, C_Token_Type_strings[token.type], STRF(token.raw));
    }

    C_Node *root = parse_c_tokens(&arena, tokens);
    print_c_ast(root, 0);

    arena_clear(&arena);
  }

  tester_summarize();

  arena_free(&arena);
}
