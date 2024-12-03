

#include <criterion/criterion.h>

#define STDR_PREFIX_BOOL
#define STDR_IMPLEMENTATION
#include "stdr.h"

Test(test_stdr, primitives) {
  cr_assert(sizeof(i8) == 1);
  cr_assert(sizeof(i16) == 2);
  cr_assert(sizeof(i32) == 4);
  cr_assert(sizeof(i64) == 8);

  cr_assert(sizeof(u8) == 1);
  cr_assert(sizeof(u16) == 2);
  cr_assert(sizeof(u32) == 4);
  cr_assert(sizeof(u64) == 8);
}

Test(test_stdr, stdr_array) {
#define N 10000

  stdr_array(i64) xs = stdr_array_new(i64, &STDR_ALLOCATOR_LIBC);
  for (usize i = 1; i <= N; i++) {
    stdr_array_append(&xs, (i64)i);
  }

  for (usize i = 0; i < stdr_array_count(xs); i++) {
    cr_assert(xs[i] == (i64)i + 1);
  }

  i64 sum = 0;
  for (usize i = 0; i < stdr_array_count(xs); i++) {
    sum += xs[i];
  }

  cr_assert(sum == (N * (N + 1)) / 2);

  stdr_array_delete(&xs);
}

Test(test_stdr, arena) {
  stdr_arena_t arena = {0};
  stdr_arena_init(&arena, 1024, &STDR_ALLOCATOR_LIBC);
  i64* a = stdr_arena_alloc_new(&arena, sizeof(*a));
  *a = 5;
  i64* b = stdr_arena_alloc_new(&arena, sizeof(*b));
  *b = 10;
  i64* c = stdr_arena_alloc_new(&arena, sizeof(*c));
  *c = 20;

  cr_assert(*a == 5);
  cr_assert(*b == 10);
  cr_assert(*c == 20);

  stdr_arena_delete(&arena);
}

stdr_bool is_whitespace(char ch) {
  if (ch == ' ') return 1;
  if (ch == '\t') return 1;
  if (ch == '\r') return 1;
  if (ch == '\n') return 1;
  if (ch == '\v') return 1;
  if (ch == '\f') return 1;
  return 0;
}

stdr_bool isn_whitespace(char ch) { return !is_whitespace(ch); }

Test(test_stdr, str) {
  stdr_arena_t arena = {0};
  stdr_arena_init(&arena, 1024, &STDR_ALLOCATOR_LIBC);

  cr_assert(stdr_str_eq(S("Hello"), S("Hello")));
  cr_assert(!stdr_str_eq(S("hello"), S("world")));

  str_t a = S("Hello, ");
  str_t b = S("World!");
  str_t c = stdr_str_concat(&arena, a, b);
  cr_assert(stdr_str_eq(c, S("Hello, World!")));

  str_t d = S("f o ob ar  ");
  str_t e = stdr_str_retain(&arena, d, isn_whitespace);
  cr_assert(stdr_str_eq(e, S("foobar")));

  str_t f = S("Per Martin");
  str_t g, h;
  stdr_str_split_at(f, 3, &g, &h);
  cr_assert(stdr_str_eq(g, S("Per")));
  cr_assert(stdr_str_eq(h, S(" Martin")));

  str_t i = S("bananas");
  cr_assert(stdr_str_contains(i, S("nanas")));
  cr_assert(!stdr_str_contains(i, S("apples")));

  usize j;
  stdr_str_find(i, S("nanas"), &j);
  cr_assert(j == 2);

  str_t k, l;
  cr_assert(stdr_str_split_once(S("cfg"), S("="), &k, &l) == STDR_FALSE);

  cr_assert(stdr_str_split_once(S("cfg=foo"), S("="), &k, &l) == STDR_TRUE);
  cr_assert(stdr_str_eq(k, S("cfg")) && stdr_str_eq(l, S("foo")));

  cr_assert(stdr_str_split_once(S("cfg=foo=bar"), S("="), &k, &l) == STDR_TRUE);
  cr_assert(stdr_str_eq(k, S("cfg")) && stdr_str_eq(l, S("foo=bar")));

  stdr_arena_delete(&arena);
}

Test(test_stdr, writable) {
  stdr_arena_ctx_init(1024, &STDR_ALLOCATOR_LIBC);

  stdr_dstr_t dstr = {0};
  stdr_dstr_init(&dstr, &STDR_ALLOCATOR_LIBC);

  stdr_dstr_append_fmt(&dstr, S("%s"), S("Hello"));
  stdr_dstr_append_fmt(&dstr, S(", %s%c"), S("World"), '!');

  cr_assert(
      stdr_str_eq(stdr_dstr_to_str(&STDR_ARENA_CTX, dstr), S("Hello, World!")));

  stdr_dstr_append_fmt(&dstr, S(" %u %i %f %x"), (u64)70, (i64)-1, (f64)420.69,
                       (u64)0xdeadbeef);

  cr_assert(stdr_str_eq(stdr_dstr_to_str(&STDR_ARENA_CTX, dstr),
                        S("Hello, World! 70 -1 420.689 deadbeef")));

  stdr_dstr_delete(&dstr);
  stdr_arena_ctx_delete();
}
