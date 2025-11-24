#define COMMON_IMPLEMENTATION
#include "../common.h"

#include "testing.h"
#include "testing.c"

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
    for EACH_INDEX(token_idx, tokens.count)
    {
      C_Token token = tokens.v[token_idx];
      printf("Token %lu: %s [ %.*s ]\n",
             token_idx, C_Token_Type_strings[token.type], STRF(token.raw));
    }

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Single character tokens"))
  {

    C_Token_Array tokens = tokenize_c_code(&arena, STR("(){}[]"));
    TEST_EVAL(tokens.count == 6);
    TEST_EVAL(tokens.v[0].type == C_TOKEN_BEGIN_PARENTHESIS);
    TEST_EVAL(tokens.v[1].type == C_TOKEN_CLOSE_PARENTHESIS);
    TEST_EVAL(tokens.v[2].type == C_TOKEN_BEGIN_CURLY_BRACE);
    TEST_EVAL(tokens.v[3].type == C_TOKEN_CLOSE_CURLY_BRACE);
    TEST_EVAL(tokens.v[4].type == C_TOKEN_BEGIN_SQUARE_BRACE);
    TEST_EVAL(tokens.v[5].type == C_TOKEN_CLOSE_SQUARE_BRACE);

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Single operators"))
  {
    C_Token_Array tokens = tokenize_c_code(&arena, STR("+ - * / % = , ; ."));
    TEST_EVAL(tokens.count == 9);
    TEST_EVAL(tokens.v[0].type == C_TOKEN_ADD);
    TEST_EVAL(tokens.v[1].type == C_TOKEN_MINUS);
    TEST_EVAL(tokens.v[2].type == C_TOKEN_STAR);
    TEST_EVAL(tokens.v[3].type == C_TOKEN_DIVIDE);
    TEST_EVAL(tokens.v[4].type == C_TOKEN_MODULO);
    TEST_EVAL(tokens.v[5].type == C_TOKEN_ASSIGN);
    TEST_EVAL(tokens.v[6].type == C_TOKEN_COMMA);
    TEST_EVAL(tokens.v[7].type == C_TOKEN_SEMICOLON);
    TEST_EVAL(tokens.v[8].type == C_TOKEN_DOT);

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Double operators"))
  {
    C_Token_Array tokens = tokenize_c_code(&arena, STR("++ -- == != <= >= && || ->"));
    TEST_EVAL(tokens.count == 9);
    TEST_EVAL(tokens.v[0].type == C_TOKEN_INCREMENT);
    TEST_EVAL(tokens.v[1].type == C_TOKEN_DECREMENT);
    TEST_EVAL(tokens.v[2].type == C_TOKEN_COMPARE_EQUAL);
    TEST_EVAL(tokens.v[3].type == C_TOKEN_COMPARE_NOT_EQUAL);
    TEST_EVAL(tokens.v[4].type == C_TOKEN_LESS_THAN_EQUAL);
    TEST_EVAL(tokens.v[5].type == C_TOKEN_GREATER_THAN_EQUAL);
    TEST_EVAL(tokens.v[6].type == C_TOKEN_LOGICAL_AND);
    TEST_EVAL(tokens.v[7].type == C_TOKEN_LOGICAL_OR);
    TEST_EVAL(tokens.v[8].type == C_TOKEN_ARROW);

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Assignment operators"))
  {
    C_Token_Array tokens = tokenize_c_code(&arena, STR("+= -= *= /= %= &= |= ^="));
    TEST_EVAL(tokens.count == 8);
    TEST_EVAL(tokens.v[0].type == C_TOKEN_ADD_ASSIGN);
    TEST_EVAL(tokens.v[1].type == C_TOKEN_SUBTRACT_ASSIGN);
    TEST_EVAL(tokens.v[2].type == C_TOKEN_MULTIPLY_ASSIGN);
    TEST_EVAL(tokens.v[3].type == C_TOKEN_DIVIDE_ASSIGN);
    TEST_EVAL(tokens.v[4].type == C_TOKEN_MODULO_ASSIGN);
    TEST_EVAL(tokens.v[5].type == C_TOKEN_AND_ASSIGN);
    TEST_EVAL(tokens.v[6].type == C_TOKEN_OR_ASSIGN);
    TEST_EVAL(tokens.v[7].type == C_TOKEN_XOR_ASSIGN);

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Bitwise operators"))
  {
    C_Token_Array tokens = tokenize_c_code(&arena, STR("& | ^ ~ < >"));
    TEST_EVAL(tokens.count == 6);
    TEST_EVAL(tokens.v[0].type == C_TOKEN_BITWISE_AND);
    TEST_EVAL(tokens.v[1].type == C_TOKEN_BITWISE_OR);
    TEST_EVAL(tokens.v[2].type == C_TOKEN_XOR);
    TEST_EVAL(tokens.v[3].type == C_TOKEN_BITWISE_NOT);
    TEST_EVAL(tokens.v[4].type == C_TOKEN_LESS_THAN);
    TEST_EVAL(tokens.v[5].type == C_TOKEN_GREATER_THAN);

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Bitwise operators"))
  {
    C_Token_Array tokens = tokenize_c_code(&arena, STR("& | ^ ~ < >"));
    TEST_EVAL(tokens.count == 6);
    TEST_EVAL(tokens.v[0].type == C_TOKEN_BITWISE_AND);
    TEST_EVAL(tokens.v[1].type == C_TOKEN_BITWISE_OR);
    TEST_EVAL(tokens.v[2].type == C_TOKEN_XOR);
    TEST_EVAL(tokens.v[3].type == C_TOKEN_BITWISE_NOT);
    TEST_EVAL(tokens.v[4].type == C_TOKEN_LESS_THAN);
    TEST_EVAL(tokens.v[5].type == C_TOKEN_GREATER_THAN);

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Control flow keywords"))
  {
    C_Token_Array tokens = tokenize_c_code(&arena, STR("if else for while do switch case default break continue return goto"));
    TEST_EVAL(tokens.count == 12);
    TEST_EVAL(tokens.v[0].type == C_TOKEN_KEYWORD_IF);
    TEST_EVAL(tokens.v[1].type == C_TOKEN_KEYWORD_ELSE);
    TEST_EVAL(tokens.v[2].type == C_TOKEN_KEYWORD_FOR);
    TEST_EVAL(tokens.v[3].type == C_TOKEN_KEYWORD_WHILE);
    TEST_EVAL(tokens.v[4].type == C_TOKEN_KEYWORD_DO);
    TEST_EVAL(tokens.v[5].type == C_TOKEN_KEYWORD_SWITCH);
    TEST_EVAL(tokens.v[6].type == C_TOKEN_KEYWORD_CASE);
    TEST_EVAL(tokens.v[7].type == C_TOKEN_KEYWORD_DEFAULT);
    TEST_EVAL(tokens.v[8].type == C_TOKEN_KEYWORD_BREAK);
    TEST_EVAL(tokens.v[9].type == C_TOKEN_KEYWORD_CONTINUE);
    TEST_EVAL(tokens.v[10].type == C_TOKEN_KEYWORD_RETURN);
    TEST_EVAL(tokens.v[11].type == C_TOKEN_KEYWORD_GOTO);

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Type keywords"))
  {
    C_Token_Array tokens = tokenize_c_code(&arena, STR("void char short int long float double unsigned signed"));
    TEST_EVAL(tokens.count == 9);
    TEST_EVAL(tokens.v[0].type == C_TOKEN_KEYWORD_VOID);
    TEST_EVAL(tokens.v[1].type == C_TOKEN_KEYWORD_CHAR);
    TEST_EVAL(tokens.v[2].type == C_TOKEN_KEYWORD_SHORT);
    TEST_EVAL(tokens.v[3].type == C_TOKEN_KEYWORD_INT);
    TEST_EVAL(tokens.v[4].type == C_TOKEN_KEYWORD_LONG);
    TEST_EVAL(tokens.v[5].type == C_TOKEN_KEYWORD_FLOAT);
    TEST_EVAL(tokens.v[6].type == C_TOKEN_KEYWORD_DOUBLE);
    TEST_EVAL(tokens.v[7].type == C_TOKEN_KEYWORD_UNSIGNED);
    TEST_EVAL(tokens.v[8].type == C_TOKEN_KEYWORD_SIGNED);

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Declaration keywords"))
  {
    C_Token_Array tokens = tokenize_c_code(&arena, STR("struct enum union typedef const static extern inline register restrict sizeof"));
    TEST_EVAL(tokens.count == 11);
    TEST_EVAL(tokens.v[0].type == C_TOKEN_KEYWORD_STRUCT);
    TEST_EVAL(tokens.v[1].type == C_TOKEN_KEYWORD_ENUM);
    TEST_EVAL(tokens.v[2].type == C_TOKEN_KEYWORD_UNION);
    TEST_EVAL(tokens.v[3].type == C_TOKEN_KEYWORD_TYPEDEF);
    TEST_EVAL(tokens.v[4].type == C_TOKEN_KEYWORD_CONST);
    TEST_EVAL(tokens.v[5].type == C_TOKEN_KEYWORD_STATIC);
    TEST_EVAL(tokens.v[6].type == C_TOKEN_KEYWORD_EXTERN);
    TEST_EVAL(tokens.v[7].type == C_TOKEN_KEYWORD_INLINE);
    TEST_EVAL(tokens.v[8].type == C_TOKEN_KEYWORD_REGISTER);
    TEST_EVAL(tokens.v[9].type == C_TOKEN_KEYWORD_RESTRICT);
    TEST_EVAL(tokens.v[10].type == C_TOKEN_KEYWORD_SIZEOF);

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Identifiers"))
  {
    C_Token_Array tokens = tokenize_c_code(&arena, STR("foo bar _test var123 _123 intensity floating"));
    TEST_EVAL(tokens.count == 7);
    TEST_EVAL(tokens.v[0].type == C_TOKEN_IDENTIFIER);
    TEST_EVAL(tokens.v[1].type == C_TOKEN_IDENTIFIER);
    TEST_EVAL(tokens.v[2].type == C_TOKEN_IDENTIFIER);
    TEST_EVAL(tokens.v[3].type == C_TOKEN_IDENTIFIER);
    TEST_EVAL(tokens.v[4].type == C_TOKEN_IDENTIFIER);
    TEST_EVAL(string_match(tokens.v[0].raw, STR("foo")));
    TEST_EVAL(string_match(tokens.v[1].raw, STR("bar")));
    TEST_EVAL(string_match(tokens.v[2].raw, STR("_test")));
    TEST_EVAL(string_match(tokens.v[3].raw, STR("var123")));
    TEST_EVAL(string_match(tokens.v[4].raw, STR("_123")));
    TEST_EVAL(string_match(tokens.v[5].raw, STR("intensity")));
    TEST_EVAL(string_match(tokens.v[6].raw, STR("floating")));

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Integer literals"))
  {
    C_Token_Array tokens = tokenize_c_code(&arena, STR("42 123u 456L 789LL 999uL 111uLL"));
    TEST_EVAL(tokens.count == 6);
    TEST_EVAL(tokens.v[0].type == C_TOKEN_LITERAL_INT);
    TEST_EVAL(tokens.v[1].type == C_TOKEN_LITERAL_UNSIGNED_INT);
    TEST_EVAL(tokens.v[2].type == C_TOKEN_LITERAL_LONG);
    TEST_EVAL(tokens.v[3].type == C_TOKEN_LITERAL_LONG_LONG);
    TEST_EVAL(tokens.v[4].type == C_TOKEN_LITERAL_UNSIGNED_LONG);
    TEST_EVAL(tokens.v[5].type == C_TOKEN_LITERAL_UNSIGNED_LONG_LONG);
    TEST_EVAL(string_match(tokens.v[0].raw, STR("42")));
    TEST_EVAL(string_match(tokens.v[1].raw, STR("123u")));
    TEST_EVAL(string_match(tokens.v[2].raw, STR("456L")));
    TEST_EVAL(string_match(tokens.v[3].raw, STR("789LL")));
    TEST_EVAL(string_match(tokens.v[4].raw, STR("999uL")));
    TEST_EVAL(string_match(tokens.v[5].raw, STR("111uLL")));
    TEST_EVAL(tokens.v[0].int_literal.v == 42);
    TEST_EVAL(tokens.v[1].int_literal.v == 123);
    TEST_EVAL(tokens.v[2].int_literal.v == 456);
    TEST_EVAL(tokens.v[3].int_literal.v == 789);
    TEST_EVAL(tokens.v[4].int_literal.v == 999);
    TEST_EVAL(tokens.v[5].int_literal.v == 111);

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Float literals"))
  {
    C_Token_Array tokens = tokenize_c_code(&arena, STR("3.14 2.5f 1.0e10 6.022E-23"));
    TEST_EVAL(tokens.count == 4);
    TEST_EVAL(tokens.v[0].type == C_TOKEN_LITERAL_DOUBLE);
    TEST_EVAL(tokens.v[1].type == C_TOKEN_LITERAL_FLOAT);
    TEST_EVAL(tokens.v[2].type == C_TOKEN_LITERAL_DOUBLE);
    TEST_EVAL(tokens.v[3].type == C_TOKEN_LITERAL_DOUBLE);
    TEST_EVAL(string_match(tokens.v[0].raw, STR("3.14")));
    TEST_EVAL(string_match(tokens.v[1].raw, STR("2.5f")));
    TEST_EVAL(string_match(tokens.v[2].raw, STR("1.0e10")));
    TEST_EVAL(string_match(tokens.v[3].raw, STR("6.022E-23")));
    TEST_EVAL(EPSILON_EQUAL(tokens.v[0].float_literal, 3.14));
    TEST_EVAL(EPSILON_EQUAL(tokens.v[1].float_literal, 2.5));
    TEST_EVAL(EPSILON_EQUAL(tokens.v[2].float_literal, (1.0 * pow(10, 10))));
    TEST_EVAL(EPSILON_EQUAL(tokens.v[3].float_literal, (6.022 * pow(10, -23))));

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("String literals"))
  {
    C_Token_Array tokens = tokenize_c_code(&arena, STR("\"hello\" \"world\" \"test\\nstring\""));
    TEST_EVAL(tokens.count == 3);
    TEST_EVAL(tokens.v[0].type == C_TOKEN_LITERAL_STRING);
    TEST_EVAL(tokens.v[1].type == C_TOKEN_LITERAL_STRING);
    TEST_EVAL(tokens.v[2].type == C_TOKEN_LITERAL_STRING);
    TEST_EVAL(string_match(tokens.v[0].raw, STR("\"hello\"")));
    TEST_EVAL(string_match(tokens.v[1].raw, STR("\"world\"")));
    TEST_EVAL(string_match(tokens.v[2].raw, STR("\"test\\nstring\"")));

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Character literals"))
  {
    C_Token_Array tokens = tokenize_c_code(&arena, STR("'a' 'b' '\\n' '\\t'"));
    TEST_EVAL(tokens.count == 4);
    TEST_EVAL(tokens.v[0].type == C_TOKEN_LITERAL_CHAR);
    TEST_EVAL(tokens.v[1].type == C_TOKEN_LITERAL_CHAR);
    TEST_EVAL(tokens.v[2].type == C_TOKEN_LITERAL_CHAR);
    TEST_EVAL(tokens.v[3].type == C_TOKEN_LITERAL_CHAR);
    TEST_EVAL(string_match(tokens.v[0].raw, STR("'a'")));
    TEST_EVAL(string_match(tokens.v[1].raw, STR("'b'")));
    TEST_EVAL(string_match(tokens.v[2].raw, STR("'\\n'")));
    TEST_EVAL(string_match(tokens.v[3].raw, STR("'\\t'")));

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Single-line comments"))
  {
    C_Token_Array tokens = tokenize_c_code(&arena, STR("foo // this is a comment\nbar"));
    TEST_EVAL(tokens.count == 2);
    TEST_EVAL(tokens.v[0].type == C_TOKEN_IDENTIFIER);
    TEST_EVAL(tokens.v[1].type == C_TOKEN_IDENTIFIER);
    TEST_EVAL(string_match(tokens.v[0].raw, STR("foo")));
    TEST_EVAL(string_match(tokens.v[1].raw, STR("bar")));

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Multi-line comments"))
  {
    C_Token_Array tokens = tokenize_c_code(&arena, STR("foo /* this is a\nmulti-line comment */ bar"));
    TEST_EVAL(tokens.count == 2);
    TEST_EVAL(tokens.v[0].type == C_TOKEN_IDENTIFIER);
    TEST_EVAL(tokens.v[1].type == C_TOKEN_IDENTIFIER);
    TEST_EVAL(string_match(tokens.v[0].raw, STR("foo")));
    TEST_EVAL(string_match(tokens.v[1].raw, STR("bar")));

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Simple expression"))
  {
    C_Token_Array tokens = tokenize_c_code(&arena, STR("x = y + 5;"));
    TEST_EVAL(tokens.count == 6);
    TEST_EVAL(tokens.v[0].type == C_TOKEN_IDENTIFIER);
    TEST_EVAL(tokens.v[1].type == C_TOKEN_ASSIGN);
    TEST_EVAL(tokens.v[2].type == C_TOKEN_IDENTIFIER);
    TEST_EVAL(tokens.v[3].type == C_TOKEN_ADD);
    TEST_EVAL(tokens.v[4].type == C_TOKEN_LITERAL_INT);
    TEST_EVAL(tokens.v[5].type == C_TOKEN_SEMICOLON);

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Simple expression"))
  {
    C_Token_Array tokens = tokenize_c_code(&arena, STR("x = y + 5;"));
    TEST_EVAL(tokens.count == 6);
    TEST_EVAL(tokens.v[0].type == C_TOKEN_IDENTIFIER);
    TEST_EVAL(tokens.v[1].type == C_TOKEN_ASSIGN);
    TEST_EVAL(tokens.v[2].type == C_TOKEN_IDENTIFIER);
    TEST_EVAL(tokens.v[3].type == C_TOKEN_ADD);
    TEST_EVAL(tokens.v[4].type == C_TOKEN_LITERAL_INT);
    TEST_EVAL(tokens.v[5].type == C_TOKEN_SEMICOLON);

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Function declaration"))
  {
    C_Token_Array tokens = tokenize_c_code(&arena, STR("int foo(int x, int y)"));
    TEST_EVAL(tokens.count == 9);
    TEST_EVAL(tokens.v[0].type == C_TOKEN_KEYWORD_INT);
    TEST_EVAL(tokens.v[1].type == C_TOKEN_IDENTIFIER);
    TEST_EVAL(tokens.v[2].type == C_TOKEN_BEGIN_PARENTHESIS);
    TEST_EVAL(tokens.v[3].type == C_TOKEN_KEYWORD_INT);
    TEST_EVAL(tokens.v[4].type == C_TOKEN_IDENTIFIER);
    TEST_EVAL(tokens.v[5].type == C_TOKEN_COMMA);
    TEST_EVAL(tokens.v[6].type == C_TOKEN_KEYWORD_INT);
    TEST_EVAL(tokens.v[7].type == C_TOKEN_IDENTIFIER);
    TEST_EVAL(tokens.v[8].type == C_TOKEN_CLOSE_PARENTHESIS);

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Struct member access"))
  {
    C_Token_Array tokens = tokenize_c_code(&arena, STR("foo.bar ptr->baz"));
    TEST_EVAL(tokens.count == 6);
    TEST_EVAL(tokens.v[0].type == C_TOKEN_IDENTIFIER);
    TEST_EVAL(tokens.v[1].type == C_TOKEN_DOT);
    TEST_EVAL(tokens.v[2].type == C_TOKEN_IDENTIFIER);
    TEST_EVAL(tokens.v[3].type == C_TOKEN_IDENTIFIER);
    TEST_EVAL(tokens.v[4].type == C_TOKEN_ARROW);
    TEST_EVAL(tokens.v[5].type == C_TOKEN_IDENTIFIER);

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Pointer operations"))
  {
    C_Token_Array tokens = tokenize_c_code(&arena, STR("*ptr &value **double_ptr"));
    TEST_EVAL(tokens.count == 7);
    TEST_EVAL(tokens.v[0].type == C_TOKEN_STAR);
    TEST_EVAL(tokens.v[1].type == C_TOKEN_IDENTIFIER);
    TEST_EVAL(tokens.v[2].type == C_TOKEN_BITWISE_AND);
    TEST_EVAL(tokens.v[3].type == C_TOKEN_IDENTIFIER);
    TEST_EVAL(tokens.v[4].type == C_TOKEN_STAR);
    TEST_EVAL(tokens.v[5].type == C_TOKEN_STAR);
    TEST_EVAL(tokens.v[6].type == C_TOKEN_IDENTIFIER);

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Array indexing"))
  {
    C_Token_Array tokens = tokenize_c_code(&arena, STR("array[0] matrix[i][j]"));
    TEST_EVAL(tokens.count == 11);
    TEST_EVAL(tokens.v[0].type == C_TOKEN_IDENTIFIER);
    TEST_EVAL(tokens.v[1].type == C_TOKEN_BEGIN_SQUARE_BRACE);
    TEST_EVAL(tokens.v[2].type == C_TOKEN_LITERAL_INT);
    TEST_EVAL(tokens.v[3].type == C_TOKEN_CLOSE_SQUARE_BRACE);
    TEST_EVAL(tokens.v[4].type == C_TOKEN_IDENTIFIER);
    TEST_EVAL(tokens.v[5].type == C_TOKEN_BEGIN_SQUARE_BRACE);
    TEST_EVAL(tokens.v[6].type == C_TOKEN_IDENTIFIER);
    TEST_EVAL(tokens.v[7].type == C_TOKEN_CLOSE_SQUARE_BRACE);
    TEST_EVAL(tokens.v[8].type == C_TOKEN_BEGIN_SQUARE_BRACE);
    TEST_EVAL(tokens.v[9].type == C_TOKEN_IDENTIFIER);
    TEST_EVAL(tokens.v[10].type == C_TOKEN_CLOSE_SQUARE_BRACE);

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Whitespace handling"))
  {
    C_Token_Array tokens = tokenize_c_code(&arena, STR("  \n\t  foo   \n  bar  \t\n"));
    TEST_EVAL(tokens.count == 2);
    TEST_EVAL(tokens.v[0].type == C_TOKEN_IDENTIFIER);
    TEST_EVAL(tokens.v[1].type == C_TOKEN_IDENTIFIER);
    TEST_EVAL(string_match(tokens.v[0].raw, STR("foo")));
    TEST_EVAL(string_match(tokens.v[1].raw, STR("bar")));

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Ternary expression"))
  {
    C_Token_Array tokens = tokenize_c_code(&arena, STR("x = x > y ? x : y;"));
    TEST_EVAL(tokens.count == 10);
    TEST_EVAL(tokens.v[0].type == C_TOKEN_IDENTIFIER);
    TEST_EVAL(tokens.v[1].type == C_TOKEN_ASSIGN);
    TEST_EVAL(tokens.v[2].type == C_TOKEN_IDENTIFIER);
    TEST_EVAL(tokens.v[3].type == C_TOKEN_GREATER_THAN);
    TEST_EVAL(tokens.v[4].type == C_TOKEN_IDENTIFIER);
    TEST_EVAL(tokens.v[5].type == C_TOKEN_QUESTION);
    TEST_EVAL(tokens.v[6].type == C_TOKEN_IDENTIFIER);
    TEST_EVAL(tokens.v[7].type == C_TOKEN_COLON);
    TEST_EVAL(tokens.v[8].type == C_TOKEN_IDENTIFIER);
    TEST_EVAL(tokens.v[9].type == C_TOKEN_SEMICOLON);

    arena_clear(&arena);
  }

  TEST_BLOCK(STR("Different base integer literals"))
  {
    C_Token_Array tokens = tokenize_c_code(&arena, STR("0x42 0x12L 0b11u 0b10 0xFA"));
    TEST_EVAL(tokens.count == 5);
    TEST_EVAL(tokens.v[0].type == C_TOKEN_LITERAL_INT);
    TEST_EVAL(tokens.v[1].type == C_TOKEN_LITERAL_LONG);
    TEST_EVAL(tokens.v[2].type == C_TOKEN_LITERAL_UNSIGNED_INT);
    TEST_EVAL(tokens.v[3].type == C_TOKEN_LITERAL_INT);
    TEST_EVAL(tokens.v[4].type == C_TOKEN_LITERAL_INT);
    TEST_EVAL(tokens.v[0].int_literal.base == 16);
    TEST_EVAL(tokens.v[1].int_literal.base == 16);
    TEST_EVAL(tokens.v[2].int_literal.base == 2);
    TEST_EVAL(tokens.v[3].int_literal.base == 2);
    TEST_EVAL(tokens.v[4].int_literal.base == 16);
    TEST_EVAL(tokens.v[0].int_literal.v == 66);
    TEST_EVAL(tokens.v[1].int_literal.v == 18);
    TEST_EVAL(tokens.v[2].int_literal.v == 3);
    TEST_EVAL(tokens.v[3].int_literal.v == 2);
    TEST_EVAL(tokens.v[4].int_literal.v == 250);

    arena_clear(&arena);
  }

  tester_summarize();

  arena_free(&arena);
}
