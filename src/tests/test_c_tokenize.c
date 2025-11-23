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
    for EACH_INDEX(token_idx, tokens.count)
    {
      C_Token token = tokens.v[token_idx];
      printf("Token %lu: %s [ %.*s ]\n",
             token_idx, C_Token_Type_strings[token.type], STRF(token.raw));
    }

    arena_clear(&arena);
  }

  {
    const char *label = "Single character tokens";

    C_Token_Array tokens = tokenize_c_code(&arena, STR("(){}[]"));
    PRINT_EVAL(label, tokens.count == 6);
    PRINT_EVAL(label, tokens.v[0].type == C_TOKEN_BEGIN_PARENTHESIS);
    PRINT_EVAL(label, tokens.v[1].type == C_TOKEN_CLOSE_PARENTHESIS);
    PRINT_EVAL(label, tokens.v[2].type == C_TOKEN_BEGIN_CURLY_BRACE);
    PRINT_EVAL(label, tokens.v[3].type == C_TOKEN_CLOSE_CURLY_BRACE);
    PRINT_EVAL(label, tokens.v[4].type == C_TOKEN_BEGIN_SQUARE_BRACE);
    PRINT_EVAL(label, tokens.v[5].type == C_TOKEN_CLOSE_SQUARE_BRACE);

    arena_clear(&arena);
  }

  {
    const char *label = "Single operators";

    C_Token_Array tokens = tokenize_c_code(&arena, STR("+ - * / % = , ; ."));
    PRINT_EVAL(label, tokens.count == 9);
    PRINT_EVAL(label, tokens.v[0].type == C_TOKEN_ADD);
    PRINT_EVAL(label, tokens.v[1].type == C_TOKEN_MINUS);
    PRINT_EVAL(label, tokens.v[2].type == C_TOKEN_STAR);
    PRINT_EVAL(label, tokens.v[3].type == C_TOKEN_DIVIDE);
    PRINT_EVAL(label, tokens.v[4].type == C_TOKEN_MODULO);
    PRINT_EVAL(label, tokens.v[5].type == C_TOKEN_ASSIGN);
    PRINT_EVAL(label, tokens.v[6].type == C_TOKEN_COMMA);
    PRINT_EVAL(label, tokens.v[7].type == C_TOKEN_SEMICOLON);
    PRINT_EVAL(label, tokens.v[8].type == C_TOKEN_DOT);

    arena_clear(&arena);
  }

  {
    const char *label = "Double operators";

    C_Token_Array tokens = tokenize_c_code(&arena, STR("++ -- == != <= >= && || ->"));
    PRINT_EVAL(label, tokens.count == 9);
    PRINT_EVAL(label, tokens.v[0].type == C_TOKEN_INCREMENT);
    PRINT_EVAL(label, tokens.v[1].type == C_TOKEN_DECREMENT);
    PRINT_EVAL(label, tokens.v[2].type == C_TOKEN_COMPARE_EQUAL);
    PRINT_EVAL(label, tokens.v[3].type == C_TOKEN_COMPARE_NOT_EQUAL);
    PRINT_EVAL(label, tokens.v[4].type == C_TOKEN_LESS_THAN_EQUAL);
    PRINT_EVAL(label, tokens.v[5].type == C_TOKEN_GREATER_THAN_EQUAL);
    PRINT_EVAL(label, tokens.v[6].type == C_TOKEN_LOGICAL_AND);
    PRINT_EVAL(label, tokens.v[7].type == C_TOKEN_LOGICAL_OR);
    PRINT_EVAL(label, tokens.v[8].type == C_TOKEN_ARROW);

    arena_clear(&arena);
  }

  {
    const char *label = "Assignment operators";

    C_Token_Array tokens = tokenize_c_code(&arena, STR("+= -= *= /= %= &= |= ^="));
    PRINT_EVAL(label, tokens.count == 8);
    PRINT_EVAL(label, tokens.v[0].type == C_TOKEN_ADD_ASSIGN);
    PRINT_EVAL(label, tokens.v[1].type == C_TOKEN_SUBTRACT_ASSIGN);
    PRINT_EVAL(label, tokens.v[2].type == C_TOKEN_MULTIPLY_ASSIGN);
    PRINT_EVAL(label, tokens.v[3].type == C_TOKEN_DIVIDE_ASSIGN);
    PRINT_EVAL(label, tokens.v[4].type == C_TOKEN_MODULO_ASSIGN);
    PRINT_EVAL(label, tokens.v[5].type == C_TOKEN_AND_ASSIGN);
    PRINT_EVAL(label, tokens.v[6].type == C_TOKEN_OR_ASSIGN);
    PRINT_EVAL(label, tokens.v[7].type == C_TOKEN_XOR_ASSIGN);

    arena_clear(&arena);
  }

  {
    const char *label = "Bitwise operators";

    C_Token_Array tokens = tokenize_c_code(&arena, STR("& | ^ ~ < >"));
    PRINT_EVAL(label, tokens.count == 6);
    PRINT_EVAL(label, tokens.v[0].type == C_TOKEN_BITWISE_AND);
    PRINT_EVAL(label, tokens.v[1].type == C_TOKEN_BITWISE_OR);
    PRINT_EVAL(label, tokens.v[2].type == C_TOKEN_XOR);
    PRINT_EVAL(label, tokens.v[3].type == C_TOKEN_BITWISE_NOT);
    PRINT_EVAL(label, tokens.v[4].type == C_TOKEN_LESS_THAN);
    PRINT_EVAL(label, tokens.v[5].type == C_TOKEN_GREATER_THAN);

    arena_clear(&arena);
  }

  {
    const char *label = "Bitwise operators";

    C_Token_Array tokens = tokenize_c_code(&arena, STR("& | ^ ~ < >"));
    PRINT_EVAL(label, tokens.count == 6);
    PRINT_EVAL(label, tokens.v[0].type == C_TOKEN_BITWISE_AND);
    PRINT_EVAL(label, tokens.v[1].type == C_TOKEN_BITWISE_OR);
    PRINT_EVAL(label, tokens.v[2].type == C_TOKEN_XOR);
    PRINT_EVAL(label, tokens.v[3].type == C_TOKEN_BITWISE_NOT);
    PRINT_EVAL(label, tokens.v[4].type == C_TOKEN_LESS_THAN);
    PRINT_EVAL(label, tokens.v[5].type == C_TOKEN_GREATER_THAN);

    arena_clear(&arena);
  }

  {
    const char *label = "Control flow keywords";

    C_Token_Array tokens = tokenize_c_code(&arena, STR("if else for while do switch case default break continue return goto"));
    PRINT_EVAL(label, tokens.count == 12);
    PRINT_EVAL(label, tokens.v[0].type == C_TOKEN_KEYWORD_IF);
    PRINT_EVAL(label, tokens.v[1].type == C_TOKEN_KEYWORD_ELSE);
    PRINT_EVAL(label, tokens.v[2].type == C_TOKEN_KEYWORD_FOR);
    PRINT_EVAL(label, tokens.v[3].type == C_TOKEN_KEYWORD_WHILE);
    PRINT_EVAL(label, tokens.v[4].type == C_TOKEN_KEYWORD_DO);
    PRINT_EVAL(label, tokens.v[5].type == C_TOKEN_KEYWORD_SWITCH);
    PRINT_EVAL(label, tokens.v[6].type == C_TOKEN_KEYWORD_CASE);
    PRINT_EVAL(label, tokens.v[7].type == C_TOKEN_KEYWORD_DEFAULT);
    PRINT_EVAL(label, tokens.v[8].type == C_TOKEN_KEYWORD_BREAK);
    PRINT_EVAL(label, tokens.v[9].type == C_TOKEN_KEYWORD_CONTINUE);
    PRINT_EVAL(label, tokens.v[10].type == C_TOKEN_KEYWORD_RETURN);
    PRINT_EVAL(label, tokens.v[11].type == C_TOKEN_KEYWORD_GOTO);

    arena_clear(&arena);
  }

  {
    const char *label = "Type keywords";

    C_Token_Array tokens = tokenize_c_code(&arena, STR("void char short int long float double unsigned signed"));
    PRINT_EVAL(label, tokens.count == 9);
    PRINT_EVAL(label, tokens.v[0].type == C_TOKEN_KEYWORD_VOID);
    PRINT_EVAL(label, tokens.v[1].type == C_TOKEN_KEYWORD_CHAR);
    PRINT_EVAL(label, tokens.v[2].type == C_TOKEN_KEYWORD_SHORT);
    PRINT_EVAL(label, tokens.v[3].type == C_TOKEN_KEYWORD_INT);
    PRINT_EVAL(label, tokens.v[4].type == C_TOKEN_KEYWORD_LONG);
    PRINT_EVAL(label, tokens.v[5].type == C_TOKEN_KEYWORD_FLOAT);
    PRINT_EVAL(label, tokens.v[6].type == C_TOKEN_KEYWORD_DOUBLE);
    PRINT_EVAL(label, tokens.v[7].type == C_TOKEN_KEYWORD_UNSIGNED);
    PRINT_EVAL(label, tokens.v[8].type == C_TOKEN_KEYWORD_SIGNED);

    arena_clear(&arena);
  }

  {
    const char *label = "Declaration keywords";

    C_Token_Array tokens = tokenize_c_code(&arena, STR("struct enum union typedef const static extern inline register restrict sizeof"));
    PRINT_EVAL(label, tokens.count == 11);
    PRINT_EVAL(label, tokens.v[0].type == C_TOKEN_KEYWORD_STRUCT);
    PRINT_EVAL(label, tokens.v[1].type == C_TOKEN_KEYWORD_ENUM);
    PRINT_EVAL(label, tokens.v[2].type == C_TOKEN_KEYWORD_UNION);
    PRINT_EVAL(label, tokens.v[3].type == C_TOKEN_KEYWORD_TYPEDEF);
    PRINT_EVAL(label, tokens.v[4].type == C_TOKEN_KEYWORD_CONST);
    PRINT_EVAL(label, tokens.v[5].type == C_TOKEN_KEYWORD_STATIC);
    PRINT_EVAL(label, tokens.v[6].type == C_TOKEN_KEYWORD_EXTERN);
    PRINT_EVAL(label, tokens.v[7].type == C_TOKEN_KEYWORD_INLINE);
    PRINT_EVAL(label, tokens.v[8].type == C_TOKEN_KEYWORD_REGISTER);
    PRINT_EVAL(label, tokens.v[9].type == C_TOKEN_KEYWORD_RESTRICT);
    PRINT_EVAL(label, tokens.v[10].type == C_TOKEN_KEYWORD_SIZEOF);

    arena_clear(&arena);
  }

  {
    const char *label = "Identifiers";

    C_Token_Array tokens = tokenize_c_code(&arena, STR("foo bar _test var123 _123 intensity floating"));
    PRINT_EVAL(label, tokens.count == 7);
    PRINT_EVAL(label, tokens.v[0].type == C_TOKEN_IDENTIFIER);
    PRINT_EVAL(label, tokens.v[1].type == C_TOKEN_IDENTIFIER);
    PRINT_EVAL(label, tokens.v[2].type == C_TOKEN_IDENTIFIER);
    PRINT_EVAL(label, tokens.v[3].type == C_TOKEN_IDENTIFIER);
    PRINT_EVAL(label, tokens.v[4].type == C_TOKEN_IDENTIFIER);
    PRINT_EVAL(label, string_match(tokens.v[0].raw, STR("foo")));
    PRINT_EVAL(label, string_match(tokens.v[1].raw, STR("bar")));
    PRINT_EVAL(label, string_match(tokens.v[2].raw, STR("_test")));
    PRINT_EVAL(label, string_match(tokens.v[3].raw, STR("var123")));
    PRINT_EVAL(label, string_match(tokens.v[4].raw, STR("_123")));
    PRINT_EVAL(label, string_match(tokens.v[5].raw, STR("intensity")));
    PRINT_EVAL(label, string_match(tokens.v[6].raw, STR("floating")));

    arena_clear(&arena);
  }

  {
    const char *label = "Integer literals";

    C_Token_Array tokens = tokenize_c_code(&arena, STR("42 123u 456L 789LL 999uL 111uLL"));
    PRINT_EVAL(label, tokens.count == 6);
    PRINT_EVAL(label, tokens.v[0].type == C_TOKEN_LITERAL_INT);
    PRINT_EVAL(label, tokens.v[1].type == C_TOKEN_LITERAL_UNSIGNED_INT);
    PRINT_EVAL(label, tokens.v[2].type == C_TOKEN_LITERAL_LONG);
    PRINT_EVAL(label, tokens.v[3].type == C_TOKEN_LITERAL_LONG_LONG);
    PRINT_EVAL(label, tokens.v[4].type == C_TOKEN_LITERAL_UNSIGNED_LONG);
    PRINT_EVAL(label, tokens.v[5].type == C_TOKEN_LITERAL_UNSIGNED_LONG_LONG);
    PRINT_EVAL(label, string_match(tokens.v[0].raw, STR("42")));
    PRINT_EVAL(label, string_match(tokens.v[1].raw, STR("123u")));
    PRINT_EVAL(label, string_match(tokens.v[2].raw, STR("456L")));
    PRINT_EVAL(label, string_match(tokens.v[3].raw, STR("789LL")));
    PRINT_EVAL(label, string_match(tokens.v[4].raw, STR("999uL")));
    PRINT_EVAL(label, string_match(tokens.v[5].raw, STR("111uLL")));
    PRINT_EVAL(label, tokens.v[0].int_literal.v == 42);
    PRINT_EVAL(label, tokens.v[1].int_literal.v == 123);
    PRINT_EVAL(label, tokens.v[2].int_literal.v == 456);
    PRINT_EVAL(label, tokens.v[3].int_literal.v == 789);
    PRINT_EVAL(label, tokens.v[4].int_literal.v == 999);
    PRINT_EVAL(label, tokens.v[5].int_literal.v == 111);

    arena_clear(&arena);
  }

  {
    const char *label = "Float literals";

    C_Token_Array tokens = tokenize_c_code(&arena, STR("3.14 2.5f 1.0e10 6.022E23"));
    PRINT_EVAL(label, tokens.count == 4);
    PRINT_EVAL(label, tokens.v[0].type == C_TOKEN_LITERAL_DOUBLE);
    PRINT_EVAL(label, tokens.v[1].type == C_TOKEN_LITERAL_FLOAT);
    PRINT_EVAL(label, tokens.v[2].type == C_TOKEN_LITERAL_DOUBLE);
    PRINT_EVAL(label, tokens.v[3].type == C_TOKEN_LITERAL_DOUBLE);
    PRINT_EVAL(label, string_match(tokens.v[0].raw, STR("3.14")));
    PRINT_EVAL(label, string_match(tokens.v[1].raw, STR("2.5f")));
    PRINT_EVAL(label, string_match(tokens.v[2].raw, STR("1.0e10")));
    PRINT_EVAL(label, string_match(tokens.v[3].raw, STR("6.022E23")));
    PRINT_EVAL(label, tokens.v[0].float_literal == 3.14);
    PRINT_EVAL(label, tokens.v[1].float_literal == 2.5);

    arena_clear(&arena);
  }

  {
    const char *label = "String literals";

    C_Token_Array tokens = tokenize_c_code(&arena, STR("\"hello\" \"world\" \"test\\nstring\""));
    PRINT_EVAL(label, tokens.count == 3);
    PRINT_EVAL(label, tokens.v[0].type == C_TOKEN_LITERAL_STRING);
    PRINT_EVAL(label, tokens.v[1].type == C_TOKEN_LITERAL_STRING);
    PRINT_EVAL(label, tokens.v[2].type == C_TOKEN_LITERAL_STRING);
    PRINT_EVAL(label, string_match(tokens.v[0].raw, STR("\"hello\"")));
    PRINT_EVAL(label, string_match(tokens.v[1].raw, STR("\"world\"")));
    PRINT_EVAL(label, string_match(tokens.v[2].raw, STR("\"test\\nstring\"")));

    arena_clear(&arena);
  }

  {
    const char *label = "Character literals";

    C_Token_Array tokens = tokenize_c_code(&arena, STR("'a' 'b' '\\n' '\\t'"));
    PRINT_EVAL(label, tokens.count == 4);
    PRINT_EVAL(label, tokens.v[0].type == C_TOKEN_LITERAL_CHAR);
    PRINT_EVAL(label, tokens.v[1].type == C_TOKEN_LITERAL_CHAR);
    PRINT_EVAL(label, tokens.v[2].type == C_TOKEN_LITERAL_CHAR);
    PRINT_EVAL(label, tokens.v[3].type == C_TOKEN_LITERAL_CHAR);
    PRINT_EVAL(label, string_match(tokens.v[0].raw, STR("'a'")));
    PRINT_EVAL(label, string_match(tokens.v[1].raw, STR("'b'")));
    PRINT_EVAL(label, string_match(tokens.v[2].raw, STR("'\\n'")));
    PRINT_EVAL(label, string_match(tokens.v[3].raw, STR("'\\t'")));

    arena_clear(&arena);
  }

  {
    const char *label = "Single-line comments";

    C_Token_Array tokens = tokenize_c_code(&arena, STR("foo // this is a comment\nbar"));
    PRINT_EVAL(label, tokens.count == 2);
    PRINT_EVAL(label, tokens.v[0].type == C_TOKEN_IDENTIFIER);
    PRINT_EVAL(label, tokens.v[1].type == C_TOKEN_IDENTIFIER);
    PRINT_EVAL(label, string_match(tokens.v[0].raw, STR("foo")));
    PRINT_EVAL(label, string_match(tokens.v[1].raw, STR("bar")));

    arena_clear(&arena);
  }

  {
    const char *label = "Multi-line comments";

    C_Token_Array tokens = tokenize_c_code(&arena, STR("foo /* this is a\nmulti-line comment */ bar"));
    PRINT_EVAL(label, tokens.count == 2);
    PRINT_EVAL(label, tokens.v[0].type == C_TOKEN_IDENTIFIER);
    PRINT_EVAL(label, tokens.v[1].type == C_TOKEN_IDENTIFIER);
    PRINT_EVAL(label, string_match(tokens.v[0].raw, STR("foo")));
    PRINT_EVAL(label, string_match(tokens.v[1].raw, STR("bar")));

    arena_clear(&arena);
  }

  {
    const char *label = "Simple expression";

    C_Token_Array tokens = tokenize_c_code(&arena, STR("x = y + 5;"));
    PRINT_EVAL(label, tokens.count == 6);
    PRINT_EVAL(label, tokens.v[0].type == C_TOKEN_IDENTIFIER);
    PRINT_EVAL(label, tokens.v[1].type == C_TOKEN_ASSIGN);
    PRINT_EVAL(label, tokens.v[2].type == C_TOKEN_IDENTIFIER);
    PRINT_EVAL(label, tokens.v[3].type == C_TOKEN_ADD);
    PRINT_EVAL(label, tokens.v[4].type == C_TOKEN_LITERAL_INT);
    PRINT_EVAL(label, tokens.v[5].type == C_TOKEN_SEMICOLON);

    arena_clear(&arena);
  }

  {
    const char *label = "Simple expression";

    C_Token_Array tokens = tokenize_c_code(&arena, STR("x = y + 5;"));
    PRINT_EVAL(label, tokens.count == 6);
    PRINT_EVAL(label, tokens.v[0].type == C_TOKEN_IDENTIFIER);
    PRINT_EVAL(label, tokens.v[1].type == C_TOKEN_ASSIGN);
    PRINT_EVAL(label, tokens.v[2].type == C_TOKEN_IDENTIFIER);
    PRINT_EVAL(label, tokens.v[3].type == C_TOKEN_ADD);
    PRINT_EVAL(label, tokens.v[4].type == C_TOKEN_LITERAL_INT);
    PRINT_EVAL(label, tokens.v[5].type == C_TOKEN_SEMICOLON);

    arena_clear(&arena);
  }

  {
    const char *label = "Function declaration";

    C_Token_Array tokens = tokenize_c_code(&arena, STR("int foo(int x, int y)"));
    PRINT_EVAL(label, tokens.count == 9);
    PRINT_EVAL(label, tokens.v[0].type == C_TOKEN_KEYWORD_INT);
    PRINT_EVAL(label, tokens.v[1].type == C_TOKEN_IDENTIFIER);
    PRINT_EVAL(label, tokens.v[2].type == C_TOKEN_BEGIN_PARENTHESIS);
    PRINT_EVAL(label, tokens.v[3].type == C_TOKEN_KEYWORD_INT);
    PRINT_EVAL(label, tokens.v[4].type == C_TOKEN_IDENTIFIER);
    PRINT_EVAL(label, tokens.v[5].type == C_TOKEN_COMMA);
    PRINT_EVAL(label, tokens.v[6].type == C_TOKEN_KEYWORD_INT);
    PRINT_EVAL(label, tokens.v[7].type == C_TOKEN_IDENTIFIER);
    PRINT_EVAL(label, tokens.v[8].type == C_TOKEN_CLOSE_PARENTHESIS);

    arena_clear(&arena);
  }

  {
    const char *label = "Struct member access";

    C_Token_Array tokens = tokenize_c_code(&arena, STR("foo.bar ptr->baz"));
    PRINT_EVAL(label, tokens.count == 6);
    PRINT_EVAL(label, tokens.v[0].type == C_TOKEN_IDENTIFIER);
    PRINT_EVAL(label, tokens.v[1].type == C_TOKEN_DOT);
    PRINT_EVAL(label, tokens.v[2].type == C_TOKEN_IDENTIFIER);
    PRINT_EVAL(label, tokens.v[3].type == C_TOKEN_IDENTIFIER);
    PRINT_EVAL(label, tokens.v[4].type == C_TOKEN_ARROW);
    PRINT_EVAL(label, tokens.v[5].type == C_TOKEN_IDENTIFIER);

    arena_clear(&arena);
  }

  {
    const char *label = "Pointer operations";

    C_Token_Array tokens = tokenize_c_code(&arena, STR("*ptr &value **double_ptr"));
    PRINT_EVAL(label, tokens.count == 7);
    PRINT_EVAL(label, tokens.v[0].type == C_TOKEN_STAR);
    PRINT_EVAL(label, tokens.v[1].type == C_TOKEN_IDENTIFIER);
    PRINT_EVAL(label, tokens.v[2].type == C_TOKEN_BITWISE_AND);
    PRINT_EVAL(label, tokens.v[3].type == C_TOKEN_IDENTIFIER);
    PRINT_EVAL(label, tokens.v[4].type == C_TOKEN_STAR);
    PRINT_EVAL(label, tokens.v[5].type == C_TOKEN_STAR);
    PRINT_EVAL(label, tokens.v[6].type == C_TOKEN_IDENTIFIER);

    arena_clear(&arena);
  }

  {
    const char *label = "Array indexing";

    C_Token_Array tokens = tokenize_c_code(&arena, STR("array[0] matrix[i][j]"));
    PRINT_EVAL(label, tokens.count == 11);
    PRINT_EVAL(label, tokens.v[0].type == C_TOKEN_IDENTIFIER);
    PRINT_EVAL(label, tokens.v[1].type == C_TOKEN_BEGIN_SQUARE_BRACE);
    PRINT_EVAL(label, tokens.v[2].type == C_TOKEN_LITERAL_INT);
    PRINT_EVAL(label, tokens.v[3].type == C_TOKEN_CLOSE_SQUARE_BRACE);
    PRINT_EVAL(label, tokens.v[4].type == C_TOKEN_IDENTIFIER);
    PRINT_EVAL(label, tokens.v[5].type == C_TOKEN_BEGIN_SQUARE_BRACE);
    PRINT_EVAL(label, tokens.v[6].type == C_TOKEN_IDENTIFIER);
    PRINT_EVAL(label, tokens.v[7].type == C_TOKEN_CLOSE_SQUARE_BRACE);
    PRINT_EVAL(label, tokens.v[8].type == C_TOKEN_BEGIN_SQUARE_BRACE);
    PRINT_EVAL(label, tokens.v[9].type == C_TOKEN_IDENTIFIER);
    PRINT_EVAL(label, tokens.v[10].type == C_TOKEN_CLOSE_SQUARE_BRACE);

    arena_clear(&arena);
  }

  {
    const char *label = "Whitespace handling";

    C_Token_Array tokens = tokenize_c_code(&arena, STR("  \n\t  foo   \n  bar  \t\n"));
    PRINT_EVAL(label, tokens.count == 2);
    PRINT_EVAL(label, tokens.v[0].type == C_TOKEN_IDENTIFIER);
    PRINT_EVAL(label, tokens.v[1].type == C_TOKEN_IDENTIFIER);
    PRINT_EVAL(label, string_match(tokens.v[0].raw, STR("foo")));
    PRINT_EVAL(label, string_match(tokens.v[1].raw, STR("bar")));

    arena_clear(&arena);
  }

  {
    const char *label = "Ternary expression";

    C_Token_Array tokens = tokenize_c_code(&arena, STR("x = x > y ? x : y;"));
    PRINT_EVAL(label, tokens.count == 10);
    PRINT_EVAL(label, tokens.v[0].type == C_TOKEN_IDENTIFIER);
    PRINT_EVAL(label, tokens.v[1].type == C_TOKEN_ASSIGN);
    PRINT_EVAL(label, tokens.v[2].type == C_TOKEN_IDENTIFIER);
    PRINT_EVAL(label, tokens.v[3].type == C_TOKEN_GREATER_THAN);
    PRINT_EVAL(label, tokens.v[4].type == C_TOKEN_IDENTIFIER);
    PRINT_EVAL(label, tokens.v[5].type == C_TOKEN_QUESTION);
    PRINT_EVAL(label, tokens.v[6].type == C_TOKEN_IDENTIFIER);
    PRINT_EVAL(label, tokens.v[7].type == C_TOKEN_COLON);
    PRINT_EVAL(label, tokens.v[8].type == C_TOKEN_IDENTIFIER);
    PRINT_EVAL(label, tokens.v[9].type == C_TOKEN_SEMICOLON);

    arena_clear(&arena);
  }

  {
    const char *label = "Different base integer literals";

    C_Token_Array tokens = tokenize_c_code(&arena, STR("0x42 0x12L 0b11u 0b10 0xFA"));
    PRINT_EVAL(label, tokens.count == 5);
    PRINT_EVAL(label, tokens.v[0].type == C_TOKEN_LITERAL_INT);
    PRINT_EVAL(label, tokens.v[1].type == C_TOKEN_LITERAL_LONG);
    PRINT_EVAL(label, tokens.v[2].type == C_TOKEN_LITERAL_UNSIGNED_INT);
    PRINT_EVAL(label, tokens.v[3].type == C_TOKEN_LITERAL_INT);
    PRINT_EVAL(label, tokens.v[4].type == C_TOKEN_LITERAL_INT);
    PRINT_EVAL(label, tokens.v[0].int_literal.base == 16);
    PRINT_EVAL(label, tokens.v[1].int_literal.base == 16);
    PRINT_EVAL(label, tokens.v[2].int_literal.base == 2);
    PRINT_EVAL(label, tokens.v[3].int_literal.base == 2);
    PRINT_EVAL(label, tokens.v[4].int_literal.base == 16);
    PRINT_EVAL(label, tokens.v[0].int_literal.v == 66);
    PRINT_EVAL(label, tokens.v[1].int_literal.v == 18);
    PRINT_EVAL(label, tokens.v[2].int_literal.v == 3);
    PRINT_EVAL(label, tokens.v[3].int_literal.v == 2);
    PRINT_EVAL(label, tokens.v[4].int_literal.v == 250);

    arena_clear(&arena);
  }

  arena_free(&arena);
}
