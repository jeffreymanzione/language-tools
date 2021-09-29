#include "lang/parser/production_parser/production_parser_semantics.h"

ImplPopulate(epsilon, const SyntaxTree *stree, SemanticAnalyzer *analyzer) {}

ImplDelete(epsilon, SemanticAnalyzer *analyzer) {}

ImplPopulate(token, const SyntaxTree *stree, SemanticAnalyzer *analyzer) {
  const SyntaxTree *tok = CHILD_SYNTAX_AT(stree, 2);
  if (!IS_TOKEN(tok)) {
    ERROR("Rule token must have a token.");
  }
  token->token_type = tok->token->text;
}

ImplDelete(token, SemanticAnalyzer *analyzer) {}

ImplPopulate(rule, const SyntaxTree *stree, SemanticAnalyzer *analyzer) {
  const SyntaxTree *rule_name = CHILD_SYNTAX_AT(stree, 2);
  if (!IS_TOKEN(rule_name)) {
    ERROR("Rule rule must have a rule_name.");
  }
  rule->rule_name = rule_name->token->text;
}

ImplDelete(rule, SemanticAnalyzer *analyzer) {}

_populate_list1(SemanticAnalyzer *analyzer, const SyntaxTree *list1,
                AList *expressions) {
  EXPECT_TYPE(list1, rule_list1);
  const SyntaxTree *first = CHILD_SYNTAX_AT(list1, 1);
  ExpressionTree *first_rule = semantic_analyzer_populate(analyzer, first);
  alist_append(expressions, &first_rule);

  const SyntaxTree *tail = CHILD_SYNTAX_AT(list1, 2);
  if (NULL != tail) {
    _populate_list1(analyzer, tail, expressions);
  }
}

void _populate_list(SemanticAnalyzer *analyzer, const SyntaxTree *child_list,
                    AList *expressions) {
  EXPECT_TYPE(child_list, rule_list);
  alist_init(expressions, ExpressionTree *, DEFAULT_ARRAY_SZ);

  const SyntaxTree *first = CHILD_SYNTAX_AT(child_list, 0);
  ExpressionTree *first_rule = semantic_analyzer_populate(analyzer, first);
  alist_append(expressions, &first_rule);

  const SyntaxTree *tail = CHILD_SYNTAX_AT(child_list, 1);
  _populate_list1(analyzer, tail, expressions);
}

ImplPopulate(and, const SyntaxTree *stree, SemanticAnalyzer *analyzer) {
  const SyntaxTree *child_list = CHILD_SYNTAX_AT(stree, 2);
  _populate_list(analyzer, child_list, &and->expressions);
}

ImplDelete(and, SemanticAnalyzer *analyzer) {
  AL_iter expressions = alist_iter(&and->expressions);
  for (; al_has(&expressions); al_inc(&expressions)) {
    ExpressionTree *etree = *(ExpressionTree **)al_value(&expressions);
    semantic_analyzer_delete(analyzer, etree);
  }
  alist_finalize(&and->expressions);
}

ImplPopulate(or, const SyntaxTree *stree, SemanticAnalyzer *analyzer) {
  const SyntaxTree *child_list = CHILD_SYNTAX_AT(stree, 2);
  _populate_list(analyzer, child_list, & or->expressions);
}

ImplDelete(or, SemanticAnalyzer *analyzer) {
  AL_iter expressions = alist_iter(& or->expressions);
  for (; al_has(&expressions); al_inc(&expressions)) {
    ExpressionTree *etree = *(ExpressionTree **)al_value(&expressions);
    semantic_analyzer_delete(analyzer, etree);
  }
  alist_finalize(& or->expressions);
}

ImplPopulate(optional, const SyntaxTree *stree, SemanticAnalyzer *analyzer) {
  const SyntaxTree *exp = CHILD_SYNTAX_AT(stree, 2);
  optional->expression = semantic_analyzer_populate(analyzer, exp);
}

ImplDelete(optional, SemanticAnalyzer *analyzer) {
  semantic_analyzer_delete(analyzer, optional->expression);
}

ImplPopulate(production_rule, const SyntaxTree *stree,
             SemanticAnalyzer *analyzer) {
  if (3 != alist_len(&stree->children)) {
    ERROR("Rule production_rule must have 3 children.");
  }
  const SyntaxTree *identifier = CHILD_SYNTAX_AT(stree, 0);
  const SyntaxTree *expression = CHILD_SYNTAX_AT(stree, 2);
  if (!IS_TOKEN(identifier)) {
    ERROR("First child of production_rule must be an identifier.");
  }
  production_rule->rule_name = TOKEN_TEXT_FOR(identifier);
  // printf("%s\n", production_rule->rule_name);
  // syntax_tree_print(expression, 0, stdout);
  // printf("\n\n");

  production_rule->expression =
      semantic_analyzer_populate(analyzer, expression);
}

ImplDelete(production_rule, SemanticAnalyzer *analyzer) {
  if (NULL != production_rule->expression) {
    semantic_analyzer_delete(analyzer, production_rule->expression);
  }
}

void _populate_production_rule_set1(SemanticAnalyzer *analyzer,
                                    const SyntaxTree *stree, AList *rules) {
  EXPECT_TYPE(stree, rule_production_rule_set1);
  AL_iter children = alist_iter(&stree->children);
  for (; al_has(&children); al_inc(&children)) {
    const SyntaxTree *st_child = *(SyntaxTree **)al_value(&children);
    if (IS_SYNTAX(st_child, rule_production_rule)) {
      ExpressionTree *exp = semantic_analyzer_populate(analyzer, st_child);
      alist_append(rules, &exp);
    } else if (IS_SYNTAX(st_child, rule_production_rule_set1)) {
      _populate_production_rule_set1(analyzer, st_child, rules);
    }
  }
}

ImplPopulate(production_rule_set, const SyntaxTree *stree,
             SemanticAnalyzer *analyzer) {
  alist_init(&production_rule_set->rules, ExpressionTree *, DEFAULT_ARRAY_SZ);
  const SyntaxTree *first = CHILD_SYNTAX_AT(stree, 0);
  ExpressionTree *first_rule = semantic_analyzer_populate(analyzer, first);
  alist_append(&production_rule_set->rules, &first_rule);
  if (alist_len(&stree->children) > 1) {
    _populate_production_rule_set1(analyzer, CHILD_SYNTAX_AT(stree, 1),
                                   &production_rule_set->rules);
  }
}

ImplDelete(production_rule_set, SemanticAnalyzer *analyzer) {
  AL_iter iter = alist_iter(&production_rule_set->rules);
  for (; al_has(&iter); al_inc(&iter)) {
    semantic_analyzer_delete(analyzer, *(ExpressionTree **)al_value(&iter));
  }
  alist_finalize(&production_rule_set->rules);
}

void production_parser_init_semantics(Map *populators, Map *deleters) {
  Register(epsilon);
  Register(token);
  Register(rule);
  Register(and);
  Register(or);
  Register(optional);
  Register(production_rule);
  Register(production_rule_set);
}