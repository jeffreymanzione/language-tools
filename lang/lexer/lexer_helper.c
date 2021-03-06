#include "lang/lexer/lexer_helper.h"

#include <stdlib.h>

#include "alloc/alloc.h"

bool is_numeric(const char c) { return ('0' <= c && '9' >= c); }

bool is_number(const char c) { return is_numeric(c) || '.' == c; }

bool is_alphabetic(const char c) {
  return ('A' <= c && 'Z' >= c) || ('a' <= c && 'z' >= c);
}

bool is_alphanumeric(const char c) {
  return is_numeric(c) || is_alphabetic(c) || '_' == c || '$' == c;
}

bool is_any_space(const char c) {
  switch (c) {
  case ' ':
  case '\t':
  case '\n':
  case '\r':
    return true;
  default:
    return false;
  }
}

bool is_whitespace(const char c) { return ' ' == c || '\t' == c || '\r' == c; }

char char_unesc(char u) {
  switch (u) {
  case 'a':
    return '\a';
  case 'b':
    return '\b';
  case 'f':
    return '\f';
  case 'n':
    return '\n';
  case 'r':
    return '\r';
  case 't':
    return '\t';
  case 'v':
    return '\v';
  case '\\':
    return '\\';
  case '\'':
    return '\'';
  case '\"':
    return '\"';
  case '\?':
    return '\?';
  default:
    return u;
  }
}

#define DEFAULT_ESCAPED_STRING_SZ 32

static bool _should_escape(char c) {
  switch (c) {
  case '\'':
  case '\"':
  case '\n':
  case '\r':
  case '\\':
    return true;
  default:
    return false;
  }
}

static char _excape_char(char c) {
  switch (c) {
  case '\n':
    return 'n';
  case '\t':
    return 't';
  case '\r':
    return 'r';
  case '\\':
    return '\\';
  default:
    return c;
  }
}

char *escape_string(const char str[]) {
  if (NULL == str) {
    return NULL;
  }
  const char *ptr = str;
  char c;
  char *escaped_str = ALLOC_ARRAY2(char, DEFAULT_ESCAPED_STRING_SZ);
  int escaped_len = 0, escaped_buffer_sz = DEFAULT_ESCAPED_STRING_SZ;
  while ('\0' != (c = *ptr)) {
    if (escaped_len > escaped_buffer_sz - 3) {
      escaped_str = REALLOC(escaped_str, char,
                            (escaped_buffer_sz += DEFAULT_ESCAPED_STRING_SZ));
    }
    if ('\r' == c) {
      ptr++;
      continue;
    }
    if (_should_escape(c)) {
      escaped_str[escaped_len++] = '\\';
    }
    escaped_str[escaped_len++] = _excape_char(c);
    ptr++;
  }
  escaped_str[escaped_len] = '\0';
  return REALLOC(escaped_str, char, escaped_len + 1);
}

char *strip_return_char(const char str[]) {
  if (NULL == str) {
    return NULL;
  }
  const char *ptr = str;
  char c;
  char *new_str = ALLOC_ARRAY2(char, DEFAULT_ESCAPED_STRING_SZ);
  int len = 0, buffer_sz = DEFAULT_ESCAPED_STRING_SZ;
  while ('\0' != (c = *ptr)) {
    if (len > buffer_sz - 3) {
      new_str =
          REALLOC(new_str, char, (buffer_sz += DEFAULT_ESCAPED_STRING_SZ));
    }
    if ('\r' == c) {
      ptr++;
      continue;
    }
    new_str[len++] = _excape_char(c);
    ptr++;
  }
  new_str[len] = '\0';
  return REALLOC(new_str, char, len + 1);
}