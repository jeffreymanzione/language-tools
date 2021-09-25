#ifndef LANGUAGE_TOOLS_LANG_PARSER_PARSER_H_
#define LANGUAGE_TOOLS_LANG_PARSER_PARSER_H_

#include "alloc/arena/arena.h"
#include "lang/lexer/token.h"
#include "struct/alist.h"
#include "struct/q.h"

typedef struct _SyntaxTree SyntaxTree;
typedef struct _Parser Parser;

typedef SyntaxTree *(*RuleFn)(Parser *parser);

struct _SyntaxTree {
  const char *production;
  bool matched, has_children;
  Token *token;
  AList children;
};

struct _Parser {
  __Arena st_arena;
  RuleFn root;
  Q *tokens;
};

extern SyntaxTree NO_MATCH;
extern SyntaxTree MATCH_EPSILON;

void parser_init(Parser *parser, RuleFn root);
SyntaxTree *parser_parse(Parser *parser, Q *tokens);
void parser_finalize(Parser *parser);
Token *parser_next(Parser *parser);
SyntaxTree *parser_create_st(Parser *parser, const char production[]);
void parser_delete_st(Parser *parser, SyntaxTree *st);
void syntax_tree_add_child(SyntaxTree *st, SyntaxTree *child);
SyntaxTree *match(Parser *parser, const char production[]);
void syntax_tree_print(const SyntaxTree *st, int level, FILE *out);

#endif /* LANGUAGE_TOOLS_LANG_PARSER_PARSER_H_ */