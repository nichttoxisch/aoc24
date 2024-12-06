#define STDR_IMPLEMENTATION
#include "stdr.h"

#ifndef DAYDIR
#define DAYDIR
#endif

typedef struct {
  i64 x, y;
} rule_t;

void parse_input(str_t input, array(rule_t) * out_rules,
                 array(array(i64)) * out_updates) {
  str_t rules_str, updates_str;
  assert(stdr_str_split_once(input, S("\n\n"), &rules_str, &updates_str));

  str_t row;
  str_t rest = rules_str;
  while (stdr_str_split_once(rest, S("\n"), &row, &rest)) {
    i64 v1 = stdr_str_parse_i64(stdr_str_drop_end(row, 3));
    i64 v2 = stdr_str_parse_i64(stdr_str_drop_begin(row, 3));
    array_append(out_rules, (rule_t){.x = v1, .y = v2});
  }

  rest = updates_str;
  usize ii = 0;
  while (stdr_str_split_once(rest, S("\n"), &row, &rest)) {
    array_append(out_updates, array_new(i64, &ALLOCATOR_LIBC));

    str_t col;
    str_t row_rest = row;
    while (stdr_str_split_once(row_rest, S(","), &col, &row_rest)) {
      array_append(&(*out_updates)[ii], stdr_str_parse_i64(col));
    }
    array_append(&(*out_updates)[ii], stdr_str_parse_i64(row_rest));
    ii++;
  }
}

bool update_is_rule_valid(array(i64) update, usize index, rule_t rule,
                          usize* out_index) {
  for (usize j = 0; j < index; j++) {
    if (update[index] != rule.x) continue;
    if (update[j] == rule.y) {
      if (out_index != NULL) *out_index = j;
      return FALSE;
    };
  }
  return TRUE;
}

bool update_is_valid_index(array(i64) update, usize index, array(rule_t) rules,
                           usize* out_index) {
  for (usize i = 0; i < array_count(rules); i++) {
    if (!update_is_rule_valid(update, index, rules[i], out_index)) return FALSE;
  }

  return TRUE;
}

bool update_is_valid(array(i64) update, array(rule_t) rules) {
  for (usize i = 1; i < array_count(update); i++) {
    if (!update_is_valid_index(update, i, rules, NULL)) return FALSE;
  }

  return TRUE;
}

i64 part_1(str_t input) {
  array(rule_t) rules = array_new(rule_t, &ALLOCATOR_LIBC);
  array(array(i64)) updates = array_new(array(i64), &ALLOCATOR_LIBC);

  parse_input(input, &rules, &updates);

  i64 sum = 0;
  for (usize i = 0; i < array_count(updates); i++) {
    array(i64) update = updates[i];
    if (update_is_valid(update, rules)) {
      sum += update[array_count(update) / 2];
    }
  }

  array_delete(&rules);
  forr(array(i64), u, updates) { array_delete(u); };
  array_delete(&updates);

  return sum;
}

i64 part_2(str_t input) {
  array(rule_t) rules = array_new(rule_t, &ALLOCATOR_LIBC);
  array(array(i64)) updates = array_new(array(i64), &ALLOCATOR_LIBC);

  parse_input(input, &rules, &updates);

  for (usize i = 0; i < array_count(updates); i++) {
    if (update_is_valid(updates[i], rules)) {
      array_delete(&updates[i]);
      updates[i] = NULL;
    }
  }

  for (usize i = 0; i < array_count(updates); i++) {
    if (updates[i] == NULL) continue;

    for (usize j = 0; j < array_count(updates[i]); j++) {
      usize idx;
      if (!update_is_valid_index(updates[i], j, rules, &idx)) {
        i64 t = updates[i][idx];
        updates[i][idx] = updates[i][j];
        updates[i][j] = t;
        i--;
        break;
      }
    }
  }

  i64 sum = 0;
  for (usize i = 0; i < array_count(updates); i++) {
    if (updates[i] == NULL) continue;
    array(i64) update = updates[i];

    // for (usize j = 0; j < array_count(update); j++) {
    //   printf("%ld,", update[j]);
    // }
    // printf("\n");

    sum += update[array_count(update) / 2];
  }

  array_delete(&rules);
  forr(array(i64), u, updates) {
    if (*u != NULL) array_delete(u);
  };
  array_delete(&updates);

  // printf("%ld\n", sum);
  return sum;
}

i32 main(void) {
  stdr_arena_ctx_init(1024 * 32, &STDR_ALLOCATOR_LIBC);
  str_t example = stdr_read_entire_file(&STDR_ARENA_CTX, DAYDIR "example.txt");
  str_t input = stdr_read_entire_file(&STDR_ARENA_CTX, DAYDIR "input.txt");

  STDR_ASSERT(part_1(example) == 143);
  STDR_ASSERT(part_1(input) == 6242);
  STDR_ASSERT(part_2(example) == 123);
  STDR_ASSERT(part_2(input) == 5169);

  stdr_arena_ctx_delete();

  return 0;
}
