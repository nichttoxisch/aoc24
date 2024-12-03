#ifndef STDR_REGEX_H_
#define STDR_REGEX_H_

#include "stdr.h"

typedef u64 regex_state_t;

enum regex_match {
  MATCH_LA,
  MATCH_LB,
  MATCH_LC,
  MATCH_LM,
  MATCH_LU,
  MATCH_LL,
  MATCH_LZ,
  MATCH_UZ,
  MATCH_COMMA,
  MATCH_WILDCARD,
  MATCH_ASTERIX,
  MATCH_PLUS,
  MATCH_PAREN_OPEN,
  MATCH_PAREN_CLOSE,
  MATCH_SKIP,
  MATCH_BSLASH,
  MATCH_DIGIT,
  MATCH_COUNT,
};

typedef struct {
  regex_state_t from;
  regex_state_t to;
  enum regex_match match;
} regex_transition_t;

typedef struct {
  stdr_array(regex_state_t) states;
  stdr_array(regex_transition_t) transitions;
  stdr_array(regex_state_t) env;
} regex_t;

void regex_delete(regex_t* regex);
void regex_generate_dot(regex_t* regex, const char* filename);
void regex_compile(regex_t* regex, regex_state_t s0, str_t rs);
regex_state_t regex_push_state(regex_t* regex);
void regex_push_transition(regex_t* regex, regex_state_t from, regex_state_t to,
                           enum regex_match match);
stdr_bool regex_match_ch(enum regex_match match, char ch);
enum regex_match regex_match_parse(char ch);
stdr_str_t regex_match_str_start(const regex_t* regex, regex_state_t begin,
                                 str_t str, usize count, char* sstr);
#endif

#ifdef STDR_REGEX_IMPLEMENTATION

const str_t REGEX_MATCH_LITERALS[] = {
    [MATCH_LA] = S("a"),         [MATCH_LB] = S("b"),
    [MATCH_LC] = S("c"),         [MATCH_LM] = S("m"),
    [MATCH_LU] = S("u"),         [MATCH_LL] = S("l"),
    [MATCH_LZ] = S("z"),         [MATCH_UZ] = S("Z"),
    [MATCH_COMMA] = S(","),      [MATCH_WILDCARD] = S("."),
    [MATCH_ASTERIX] = S("*"),    [MATCH_PLUS] = S("+"),
    [MATCH_PAREN_OPEN] = S("("), [MATCH_PAREN_CLOSE] = S(")"),
    [MATCH_SKIP] = S("SKIP"),    [MATCH_BSLASH] = S("\\"),
    [MATCH_DIGIT] = S("DIGIT")};

regex_state_t regex_push_state(regex_t* regex) {
  if (regex->states == NULL) {
    regex->states = stdr_array_new(regex_state_t, &STDR_ALLOCATOR_LIBC);
  }

  regex_state_t s0 = (regex_state_t)stdr_array_count(regex->states);
  stdr_array_append(&regex->states, s0);
  return s0;
}

void regex_push_transition(regex_t* regex, regex_state_t from, regex_state_t to,
                           enum regex_match match) {
  stdr_array_append(
      &regex->transitions,
      (regex_transition_t){.from = from, .to = to, .match = match});
}

stdr_bool regex_match_ch(enum regex_match match, char ch) {
  if (match == MATCH_WILDCARD) return TRUE;
  if (match == MATCH_DIGIT) return (ch >= '0' && ch <= '9');
  if (REGEX_MATCH_LITERALS[match].size != 1) assert(0);
  return REGEX_MATCH_LITERALS[match].ptr[0] == ch;

  assert(0 && "unreachable");
}

stdr_str_t regex_match_str_start(const regex_t* regex, regex_state_t begin,
                                 str_t str, usize count, char* sstr) {
  STDR_ASSERT(stdr_array_count(regex->states) >= 2);

  if (begin == stdr_array_last(regex->states))
    return (stdr_str_t){.size = count, .ptr = sstr};

  for (usize i = 0; i < stdr_array_count(regex->transitions); i++) {
    regex_transition_t trans = regex->transitions[i];
    if (trans.from != begin) continue;
    // stdr_stdout_fmt(S("[DEBUG] %u -> %u - %s - %u \n"), trans.from, trans.to,
    //                 REGEX_MATCH_LITERALS[trans.match], count);
    if (trans.match == MATCH_SKIP) {
      stdr_str_t d =
          regex_match_str_start(regex, trans.to, str, count + 1, sstr);
      if (d.ptr != NULL) return d;
    }

    if (!regex_match_ch(trans.match, str.ptr[0])) continue;

    stdr_str_t d = regex_match_str_start(
        regex, trans.to, stdr_str_drop_begin(str, 1), count + 1, sstr);
    if (d.ptr != NULL) return d;
  }

  return (stdr_str_t){.ptr = NULL, .size = 0};
}

void regex_delete(regex_t* regex) {
  stdr_array_delete(&regex->states);
  stdr_array_delete(&regex->transitions);
  stdr_array_delete(&regex->env);
}

void regex_generate_dot(regex_t* regex, const char* filename) {
  FILE* f = fopen(filename, "w+");
  assert(f != NULL);
  fprintf(f, "digraph regex {\n");
  fprintf(f, "  rankdir=\"LR\";\n");
  for (usize i = 0; i < stdr_array_count(regex->transitions); i++) {
    regex_transition_t trans = regex->transitions[i];

    fprintf(f, "s%ld -> s%ld [ label=\"%s\"]\n", trans.from, trans.to,
            REGEX_MATCH_LITERALS[trans.match].ptr);
  }
  fprintf(f, "}\n");
  fclose(f);

  stdr_cmd_t cmd = {0};
  stdr_cmd_append_all(&cmd, S("dot"), S("-Tsvg"), S("regex.dot"), S("-o"),
                      S("regex.svg"));
  stdr_cmd_run(&cmd);
  stdr_cmd_delete(&cmd);
}

enum regex_match regex_match_parse(char ch) {
  for (usize i = 0; i < MATCH_COUNT; i++) {
    if (REGEX_MATCH_LITERALS[i].size != 1) continue;

    if (ch == REGEX_MATCH_LITERALS[i].ptr[0]) {
      return (enum regex_match)i;
    }
  }

  printf("[ERROR] Unknown character '%c' (%d)\n", ch, ch);
  assert(0);
}

void regex_compile(regex_t* regex, regex_state_t s0, str_t rs) {
  if (rs.size == 0) return;
  if (regex->env == NULL) {
    regex->env = stdr_array_new(regex_state_t, &STDR_ALLOCATOR_LIBC);
  }
  if (regex->transitions == NULL) {
    regex->transitions =
        stdr_array_new(regex_transition_t, &STDR_ALLOCATOR_LIBC);
  }

  enum regex_match match = regex_match_parse(rs.ptr[0]);
  switch (match) {
    case MATCH_BSLASH: {
      if (rs.ptr[1] == 'd') {
        regex_state_t n = regex_push_state(regex);
        regex_push_transition(regex, s0, n, MATCH_DIGIT);
        regex_compile(regex, n, stdr_str_drop_begin(rs, 2));
      } else {
        assert(0);
      }
    } break;
    case MATCH_PLUS: {
      regex_transition_t t = stdr_array_last(regex->transitions);
      regex_push_transition(regex, s0, s0, t.match);
      regex_compile(regex, s0, stdr_str_drop_begin(rs, 1));
    } break;
    case MATCH_ASTERIX: {
      regex_state_t s1 = stdr_array_pre_last(regex->states);
      regex_state_t n = regex_push_state(regex);
      regex_push_transition(regex, s1, n, MATCH_SKIP);

      regex_transition_t t0 = stdr_array_pre_last(regex->transitions);
      regex_push_transition(regex, s0, s0, t0.match);
      regex_push_transition(regex, s0, n, MATCH_SKIP);
      regex_compile(regex, n, stdr_str_drop_begin(rs, 1));
    } break;
    default: {
      regex_state_t n = regex_push_state(regex);
      regex_push_transition(regex, s0, n, match);
      regex_compile(regex, n, stdr_str_drop_begin(rs, 1));
    } break;
  }
}

#endif
#undef STDR_REGEX_IMPLEMENTATION
