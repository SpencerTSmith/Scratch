#define COMMON_IMPLEMENTATION
#include "../common.h"

#include "testing.h"
#include "testing.c"

#include "../c_tokenize.c"
#include "../c_parse.c"

static
void print_c_ast(C_Node *node, isize prev_depth, isize depth)
{
  if (node)
  {
    if (depth != 0)
    {
      for (isize i = 0; i < prev_depth; i++)
      {
        printf(" ");
      }
      printf("L");
    }

    for (isize i = 0; i < depth - 1; i++)
    {
      printf("-");
    }

    if (depth != 0)
    {
      printf("> ");
    }

    switch (node->type)
    {
      default: { printf("%s", C_Node_Type_strings[node->type]); } break;

      case C_NODE_LITERAL:
      {
        C_Literal literal = node->literal;
        switch (literal.type)
        {
          default: { LOG_ERROR("Invalid literal type"); } break;

          case C_LITERAL_CHARACTER:
          {
            printf("%c", literal.character);
          }
          break;
          case C_LITERAL_STRING:
          {
            printf("%*.s", STRF(literal.string));
          }
          break;
          case C_LITERAL_INTEGER:
          {
            // TODO: account for flags better
            if (literal.flags & C_LITERAL_FLAG_UNSIGNED)
            {
              printf("%lu", literal.integer.v);
            }
            else
            {
              printf("%ld", (i64) literal.integer.v);
            }
          }
          break;
          case C_LITERAL_FLOATING:
          {
            printf("%f", literal.floating);
          }
          break;
        }
      }
      break;
      case C_NODE_BINARY:
      {
        C_Binary binary = node->binary;
        switch (binary)
        {
          default: { LOG_ERROR("Invalid binary type"); } break;

          case C_BINARY_ADD:                { printf("+"); } break;
          case C_BINARY_SUBTRACT:           { printf("-"); } break;
          case C_BINARY_MULTIPLY:           { printf("*"); } break;
          case C_BINARY_DIVIDE:             { printf("/"); } break;
          case C_BINARY_MODULO:             { printf("%%"); } break;
          case C_BINARY_XOR:                { printf("^"); } break;
          case C_BINARY_BITWISE_AND:        { printf("&"); } break;
          case C_BINARY_BITWISE_OR:         { printf("|"); }  break;
          case C_BINARY_ACCESS:             { printf("."); } break;
          case C_BINARY_POINTER_ACCESS:     { printf("->"); } break;
          case C_BINARY_ARRAY_ACCESS:       { printf("[]"); } break;
          case C_BINARY_COMPARE_EQUAL:      { printf("=="); } break;
          case C_BINARY_COMPARE_NOT_EQUAL:  { printf("!="); } break;
          case C_BINARY_LESS_THAN:          { printf("<"); } break;
          case C_BINARY_LESS_THAN_EQUAL:    { printf("<="); } break;
          case C_BINARY_GREATER_THAN:       { printf(">"); } break;
          case C_BINARY_GREATER_THAN_EQUAL: { printf(">="); } break;
          case C_BINARY_LOGICAL_AND:        { printf("&&"); } break;
          case C_BINARY_LOGICAL_OR:         { printf("||"); } break;
          case C_BINARY_LEFT_SHIFT:         { printf("<<"); } break;
          case C_BINARY_RIGHT_SHIFT:        { printf(">>"); } break;
          case C_BINARY_ASSIGN:             { printf("="); } break;
          case C_BINARY_ADD_ASSIGN:         { printf("+="); } break;
          case C_BINARY_SUBTRACT_ASSIGN:    { printf("-="); } break;
          case C_BINARY_MULTIPLY_ASSIGN:    { printf("*="); } break;
          case C_BINARY_DIVIDE_ASSIGN:      { printf("/="); } break;
          case C_BINARY_MODULO_ASSIGN:      { printf("%%="); } break;
          case C_BINARY_AND_ASSIGN:         { printf("&="); } break;
          case C_BINARY_OR_ASSIGN:          { printf("|="); } break;
          case C_BINARY_XOR_ASSIGN:         { printf("^="); } break;
          case C_BINARY_LEFT_SHIFT_ASSIGN:  { printf("<<="); } break;
          case C_BINARY_RIGHT_SHIFT_ASSIGN: { printf(">>="); } break;
        }
      }
      break;
      case C_NODE_UNARY:
      {
        C_Unary unary = node->unary;
        switch (unary)
        {
          default: { LOG_ERROR("Invalid unary type"); } break;

          case C_UNARY_PRE_INCREMENT:  { printf("pre++"); } break;
          case C_UNARY_PRE_DECREMENT:  { printf("pre--"); } break;
          case C_UNARY_POST_INCREMENT: { printf("post++"); } break;
          case C_UNARY_POST_DECREMENT: { printf("post--"); } break;
          case C_UNARY_NEGATE:         { printf("-"); } break;
          case C_UNARY_REFERENCE:      { printf("&"); } break;
          case C_UNARY_DEREFERENCE:    { printf("*"); } break;
          case C_UNARY_BITWISE_NOT:    { printf("~"); }  break;
          case C_UNARY_LOGICAL_NOT:    { printf("!"); } break;
        }
      }
      break;
      case C_NODE_VARIABLE:
      case C_NODE_TYPE:
      {
        printf("%.*s", STRF(node->name));
      }
      break;
      case C_NODE_FUNCTION_CALL:
      {
        printf("CALL - %.*s", STRF(node->name));
      }
      break;
    }
    printf("\n");

    for (C_Node *cursor = node->first_child; cursor; cursor = cursor->next_sibling)
    {
      print_c_ast(cursor, depth, depth + 1);
    }
  }
}

int main(int argc, char **argv)
{
  Arena arena = arena_make();
  {
    String sample_program =
      STR(
        // "int foo;\n"
        // "float boo;\n"
        // "int bar = 1;\n"
        // "int baz = -1;\n"
        // "int baz = -1 + 1;\n"
        // "int baz = -(1 + 1);\n"
        // "int boo = 1 + 3 + 1;\n"
        // "int boo = 1 * 3 + 1;\n"
        // "int right = 1 << 2;"
        // "int left = 1 >> 2;"
        // "int ban = bar++;\n"
        // // "int ban = ++bar;\n"
        // "int ban = bar--;\n"
        // "int par = 1 + (1 + 1) + (1 + 1);\n"
        // "int par = 1 + (1 + 1) + bar++;\n"
        // "int prec = a > b + c * d + e;\n"
        // "int call = func();\n"
        // "int call = func2(a, a + b, func());\n"
        // "int ref = &a + 1;\n"
        // "int ref = &a;\n"
        // "int tern = a ? b : c ? d : e;\n"
        // "int tern = a + b ? c : d;\n"
        // "int assign = a = b;\n"
        // "int tern_assign = a = a ? b : c ? d : e;\n"
        "int array = 1 + a[10];\n"
      );

    int a[100] = {0};
    int array = a[10]++;

    C_Token_Array tokens = tokenize_c_code(&arena, sample_program);
    // for EACH_INDEX(token_idx, tokens.count)
    // {
    //   C_Token token = tokens+.v[token_idx];
    //   printf("Token %lu: %s [ %.*s ]\n",
    //          token_idx, C_Token_Type_strings[token.type], STRF(token.raw));
    // }

    C_Node *root = parse_c_tokens(&arena, tokens);
    print_c_ast(root, 0, 0);

    arena_clear(&arena);
  }

  // tester_summarize();

  arena_free(&arena);
}
