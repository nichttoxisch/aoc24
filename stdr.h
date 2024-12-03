#ifndef STC_H_
#define STC_H_

#define str_t stdr_str_t

typedef float stdr_f32;
typedef double stdr_f64;

typedef signed char stdr_i8;
typedef signed short int stdr_i16;
typedef signed int stdr_i32;
typedef signed long stdr_i64;

typedef unsigned char stdr_u8;
typedef unsigned short stdr_u16;
typedef unsigned int stdr_u32;
typedef unsigned long stdr_u64;

typedef stdr_i64 stdr_isize;
typedef stdr_u64 stdr_usize;
typedef stdr_u64 stdr_uptr;

typedef char stdr_bool;
#define STDR_TRUE (stdr_bool)(0 == 0)
#define STDR_FALSE (stdr_bool)(0 == 1)

#ifndef STDR_PREFIX_BOOL
#define TRUE STDR_TRUE
#define FALSE STDR_FALSE

#define bool stdr_bool
#else

#endif

#ifndef STDR_PREFIX_TYPES

#define f32 stdr_f32
#define f64 stdr_f64

#define i8 stdr_i8
#define i16 stdr_i16
#define i32 stdr_i32
#define i64 stdr_i64

#define u8 stdr_u8
#define u16 stdr_u16
#define u32 stdr_u32
#define u64 stdr_u64

#define isize stdr_isize
#define usize stdr_usize
#define uptr stdr_uptr

typedef char* stdr_cstr_t;

#endif

#ifndef STDR_PREFIX

#define array stdr_array
#define array_new stdr_array_new
#define array_count stdr_array_count
#define array_append stdr_array_append
#define array_delete stdr_array_delete
#define array_last stdr_array_last
#define forr stdr_forr

#endif

#ifdef CC
#define STDR_CC CC
#else
#define STDR_CC "cc"
#endif

// * allocator

typedef void* (*stdr_allocator_new_handler_t)(stdr_usize size);
typedef void (*stdr_allocator_delete_handler_t)(void* addr);

typedef struct {
  stdr_allocator_new_handler_t new;
  stdr_allocator_delete_handler_t delete;
} stdr_allocator_t;

extern stdr_allocator_t STDR_ALLOCATOR_LIBC;
extern stdr_allocator_t STDR_ALLOCATOR_ARENA_CTX;

// math

#define STDR_ABS(x) (((x) >= 0) ? (x) : -(x))

// dynamic array

typedef struct {
  const stdr_allocator_t* allocator;

  stdr_usize item_size;
  stdr_usize count;
  stdr_usize capacity;
} stdr_array_header_t;

#define stdr_array(T) T*

#define stdr_forr(T, e, arr) \
  for (T* e = &(arr)[0]; e < &arr[array_count(arr)]; e++)

#define stdr_array_new(T, allocator) (T*)stdr_array_init(sizeof(T), allocator)
void* stdr_array_init(stdr_usize item_size, const stdr_allocator_t* allocator);
stdr_usize stdr_array_count(const void* arr);
stdr_usize stdr_array_capacity(const void* arr);
#define stdr_array_delete(arr) __stdr_array_delete((void**)arr)
void __stdr_array_delete(void** arr);
stdr_array_header_t* stdr_array_header(const void* arr);
void* stdr_array_realloc(void* arr, stdr_usize new_capacity);
void stdr_array_reset(void* arr);
#define stdr_array_last(arr) (arr)[stdr_array_count((arr)) - 1]
#define stdr_array_pre_last(arr) (arr)[stdr_array_count((arr)) - 2]

#define stdr_array_append(arr, ...)                                      \
  do {                                                                   \
    if (stdr_array_count(*(arr)) >= stdr_array_capacity(*(arr))) {       \
      stdr_usize capacity =                                              \
          (stdr_array_capacity(*(arr)) == 0)                             \
              ? (stdr_usize)4                                            \
              : (stdr_usize)((float)stdr_array_capacity(*(arr)) * 1.5f); \
      *(arr) = stdr_array_realloc(*(arr), capacity);                     \
    }                                                                    \
    (*(arr))[stdr_array_count(*(arr))] = (__VA_ARGS__);                  \
    stdr_array_header(*(arr))->count++;                                  \
  } while (0)

#define stdr_array_filter_coun

// arena

typedef struct {
  const stdr_allocator_t* allocator;
  stdr_uptr base;
  stdr_uptr pos;
  stdr_usize size;
} stdr_arena_t;

void stdr_arena_init(stdr_arena_t* arena, stdr_usize size,
                     const stdr_allocator_t* allocator);
void stdr_arena_new_static(stdr_arena_t* arena, void* buf, stdr_usize buf_size);
void* stdr_arena_alloc_new(stdr_arena_t* arena, stdr_usize size);
void stdr_arena_reset(stdr_arena_t* arena);
void stdr_arena_delete(stdr_arena_t* arena);
void stdr_arena_alloc_delete(stdr_arena_t* arena, void* ptr);

static stdr_arena_t STDR_ARENA_CTX;

void stdr_arena_ctx_init(stdr_usize size, const stdr_allocator_t* allocator);
void stdr_arena_ctx_init_static(void* buf, stdr_usize buf_size);
void* stdr_arena_ctx_alloc_new(stdr_usize size);
void stdr_arena_ctx_alloc_delete(void* ptr);
void stdr_arena_ctx_delete(void);

// * string

typedef struct {
  char* ptr;
  stdr_usize size;
} stdr_str_t;

#define STDR_FMTS "%.*s"
#define STDR_FMTSARG(arg) (int)arg.size, arg.ptr
#define S(s) \
  (stdr_str_t) { .ptr = s, .size = sizeof(s) - 1, }

stdr_str_t stdr_cstr_to_str(stdr_arena_t* arena, stdr_cstr_t cstr);
stdr_cstr_t stdr_str_to_cstr(stdr_arena_t* arena, stdr_str_t a);

stdr_str_t stdr_str_truncate(stdr_arena_t* arena, stdr_str_t a,
                             stdr_usize size);
stdr_str_t stdr_str_retain(stdr_arena_t* arena, stdr_str_t a,
                           stdr_bool (*f)(char));
stdr_str_t stdr_str_concat(stdr_arena_t* arena, stdr_str_t a, stdr_str_t b);
#define stdr_str_concat_all(arena, str, ...) \
  stdr_str_concat_all_impl(arena, str, __VA_ARGS__, (stdr_str_t){.ptr = NULL})
stdr_str_t stdr_str_concat_all_impl(stdr_arena_t* arena, stdr_str_t a, ...);
void stdr_str_split_at(stdr_str_t a, stdr_usize mid, stdr_str_t* out1,
                       stdr_str_t* out2);
stdr_bool stdr_str_split_once(stdr_str_t a, stdr_str_t pattern,
                              stdr_str_t* out1, stdr_str_t* out2);
stdr_bool stdr_str_contains(stdr_str_t a, stdr_str_t pattern);
stdr_bool stdr_str_starts_with(stdr_str_t a, stdr_str_t pattern);
stdr_bool stdr_str_ends_with(stdr_str_t a, stdr_str_t pattern);
stdr_bool stdr_str_find(stdr_str_t a, stdr_str_t pattern,
                        stdr_usize* out_index);
stdr_bool stdr_str_eq(stdr_str_t a, stdr_str_t b);
stdr_str_t stdr_str_drop_begin(stdr_str_t str, stdr_usize count);
stdr_str_t stdr_str_drop_end(stdr_str_t str, stdr_usize count);
stdr_str_t stdr_read_entire_file(stdr_arena_t* arena,
                                 const stdr_cstr_t filename);
stdr_i64 stdr_str_parse_i64(stdr_str_t str);

// * writable

typedef void (*stdr_writable_write_handler_t)(void* ctx, stdr_str_t str);

typedef struct {
  void* ctx;
  stdr_writable_write_handler_t write;
} stdr_writable_t;

extern stdr_writable_t STDR_WRITABLE_STDOUT;
extern stdr_writable_t STDR_WRITABLE_STDERR;

void stdr_writable_fmt(const stdr_writable_t* writable, stdr_str_t fmt, ...);
#define stdr_stdout_fmt(fmt, ...) \
  stdr_writable_fmt(&STDR_WRITABLE_STDOUT, fmt, __VA_ARGS__)
#define stdr_stderr_fmt(fmt, ...) \
  stdr_writable_fmt(&STDR_WRITABLE_STDERR, fmt, __VA_ARGS__)

// dstr

typedef struct {
  const stdr_allocator_t* allocator;

  char* ptr;
  stdr_usize size;
  stdr_usize capacity;
} stdr_dstr_t;

#define stdr_dstr_append_fmt(dstr, fmt, ...)                              \
  do {                                                                    \
    const stdr_writable_t writable = {.ctx = (dstr),                      \
                                      .write = stdr_writable_dstr_write}; \
    stdr_writable_fmt(&writable, fmt, __VA_ARGS__);                       \
  } while (0)

void stdr_dstr_init(stdr_dstr_t* dstr, const stdr_allocator_t* allocator);
void stdr_dstr_delete(stdr_dstr_t* dstr);
void stdr_dstr_append(stdr_dstr_t* dstr, char ch);
void stdr_dstr_append_str(stdr_dstr_t* dstr, stdr_str_t str);
stdr_str_t stdr_dstr_to_str(stdr_arena_t* arena, const stdr_dstr_t dstr);

// cmd and build

typedef struct {
  stdr_array(stdr_str_t) cmd;
} stdr_cmd_t;

void stdr_cmd_append(stdr_cmd_t* cmd, stdr_str_t str);
#define stdr_cmd_append_all(cmd, ...) \
  stdr_cmd_append_all_impl(cmd, __VA_ARGS__, (stdr_str_t){.ptr = NULL})
void stdr_cmd_run(stdr_cmd_t* cmd);
void stdr_cmd_reset(stdr_cmd_t* cmd);
void stdr_cmd_run_reset(stdr_cmd_t* cmd);
void stdr_cmd_append_all_impl(stdr_cmd_t* cmd, stdr_str_t str, ...);
void stdr_cmd_delete(const stdr_cmd_t* cmd);
void stdr_cmd_log(stdr_cmd_t* cmd, const stdr_writable_t* writable);

void stdr_build_self(stdr_str_t filename, int argc, stdr_cstr_t* argv);
#define STDR_BUILD_SELF(argc, argv) stdr_build_self(S(__FILE__), argc, argv);
stdr_bool stdr_build_is_current(stdr_str_t infile, stdr_str_t outfile);

#endif

#ifdef STDR_IMPLEMENTATION

#define _GNU_SOURCE
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#define STDR_ASSERT assert

// writable

void stdr_writable_stdio_write(void* ctx, stdr_str_t str) {
  (void)ctx;
  for (usize i = 0; i < str.size; i++) {
    putc(str.ptr[i], stdout);
  }
}

void stdr_writable_stderr_write(void* ctx, stdr_str_t str) {
  (void)ctx;
  for (usize i = 0; i < str.size; i++) {
    putc(str.ptr[i], stderr);
  }
}

stdr_writable_t STDR_WRITABLE_STDOUT = {
    .ctx = NULL,
    .write = stdr_writable_stdio_write,
};

stdr_writable_t STDR_WRITABLE_STDERR = {
    .ctx = NULL,
    .write = stdr_writable_stderr_write,
};

void stdr_writable_write_ch(const stdr_writable_t* writable, char ch) {
  stdr_str_t str = {.ptr = &ch, .size = 1};
  writable->write(writable->ctx, str);
}

void stdr_writable_write_u64(const stdr_writable_t* writable, stdr_u64 x,
                             stdr_u64 base) {
  char buf[128] = {0};
  const usize buf_size = sizeof(buf) / sizeof(*buf);
  usize i = buf_size;
  static char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
  STDR_ASSERT(base <= sizeof(digits));
  do {
    u64 n = x % base;
    x = x / base;
    i -= 1;
    buf[i] = digits[n];
  } while (x != 0);
  stdr_str_t str = {.ptr = &buf[i], .size = buf_size - i};
  writable->write(writable->ctx, str);
}

void stdr_writable_write_i64(const stdr_writable_t* writable, stdr_i64 x,
                             stdr_u64 base) {
  if (x < 0) {
    x *= -1;
    stdr_writable_write_ch(writable, '-');
  }
  stdr_writable_write_u64(writable, (u64)x, base);
}

void stdr_writable_write_f64(const stdr_writable_t* writable, stdr_f64 x,
                             usize precision) {
  if (x < 0) {
    x *= -1;
    stdr_writable_write_ch(writable, '-');
  }

  stdr_u64 integer_part = (stdr_u64)x;
  stdr_f64 fractional_part = x - (stdr_f64)integer_part;

  stdr_writable_write_u64(writable, integer_part, 10);

  if (fractional_part > 0) {
    stdr_writable_write_ch(writable, '.');
  }

  for (usize i = 0; i < precision; i++) {
    fractional_part *= 10;
    stdr_u64 digit = (stdr_u64)fractional_part;
    stdr_writable_write_u64(writable, digit, 10);
    fractional_part -= (stdr_f64)digit;
  }
}

void stdr_writable_fmt(const stdr_writable_t* writable, stdr_str_t fmt, ...) {
  va_list args;
  va_start(args, fmt);

  for (usize i = 0; i < fmt.size; i++) {
    if (fmt.ptr[i] == '%' && fmt.ptr[i + 1] != '\0') {
      i++;
      switch (fmt.ptr[i]) {
        case 's': {
          const stdr_str_t str = va_arg(args, stdr_str_t);
          writable->write(writable->ctx, str);
        } break;
        case 'i': {
          stdr_i64 num = va_arg(args, stdr_i64);
          stdr_writable_write_i64(writable, num, 10);
        } break;
        case 'u': {
          stdr_u64 num = va_arg(args, stdr_u64);
          stdr_writable_write_u64(writable, num, 10);
        } break;
        case 'x': {
          stdr_u64 num = va_arg(args, stdr_u64);
          stdr_writable_write_u64(writable, num, 16);
        } break;
        case 'f': {
          stdr_f64 num = va_arg(args, stdr_f64);
          stdr_writable_write_f64(writable, num, 3);
        } break;
        case 'c': {
          char ch = (char)va_arg(args, stdr_i32);
          stdr_writable_write_ch(writable, ch);
        } break;
        default: {
          stdr_writable_write_ch(writable, '%');
          stdr_writable_write_ch(writable, fmt.ptr[i]);
        } break;
      }
    } else {
      stdr_writable_write_ch(writable, fmt.ptr[i]);
    }
  }

  va_end(args);
}

void stdr_writable_dstr_write(void* ctx, stdr_str_t str) {
  stdr_dstr_t* dstr = (stdr_dstr_t*)ctx;
  stdr_dstr_append_str(dstr, str);
}

// * Allocator

stdr_allocator_t STDR_ALLOCATOR_LIBC = {
    .new = malloc,
    .delete = free,
};

stdr_allocator_t STDR_ALLOCATOR_ARENA_CTX = {
    .new = stdr_arena_ctx_alloc_new,
    .delete = stdr_arena_ctx_alloc_delete,
};

// * Dynamic Array

void stdr_array_reset(void* arr) {
  stdr_array_header_t* h = stdr_array_header(arr);
  h->count = 0;
}

stdr_array_header_t* stdr_array_header(const void* arr) {
  stdr_array_header_t* header = ((stdr_array_header_t*)(arr)) - 1;
  return header;
}

void __stdr_array_delete(void** arr) {
  stdr_array_header_t* header = stdr_array_header(*arr);
  header->allocator->delete (header);
  *arr = NULL;
}

stdr_usize stdr_array_count(const void* arr) {
  return stdr_array_header(arr)->count;
}

stdr_usize stdr_array_capacity(const void* arr) {
  return stdr_array_header(arr)->capacity;
}

void stdr_array_alloc(stdr_usize size_bytes, stdr_array_header_t** out_header,
                      void** out_items, const stdr_allocator_t* allocator) {
  *out_header = allocator->new (sizeof(stdr_array_header_t) + size_bytes);
  *out_items = (*out_header) + 1;
}

void* stdr_array_init(stdr_usize item_size, const stdr_allocator_t* allocator) {
  if (allocator == NULL) allocator = &STDR_ALLOCATOR_LIBC;

  stdr_array_header_t* header = NULL;
  void* items = NULL;

  stdr_array_alloc(0, &header, &items, allocator);

  header->allocator = allocator;
  header->item_size = item_size;
  header->count = 0;
  header->capacity = 0;

  return items;
}

void* stdr_array_realloc(void* arr, stdr_usize new_capacity) {
  stdr_array_header_t* header_old = stdr_array_header(arr);

  stdr_array_header_t* header = NULL;
  void* items = NULL;
  stdr_array_alloc(new_capacity * header_old->item_size, &header, &items,
                   header_old->allocator);

  header->allocator = header_old->allocator;
  header->count = header_old->count;
  header->item_size = header_old->item_size;
  header->capacity = new_capacity;

  memcpy(items, arr, header_old->count * header_old->item_size);

  __stdr_array_delete(&arr);

  return items;
}

// * Arena

void stdr_arena_init(stdr_arena_t* arena, stdr_usize size,
                     const stdr_allocator_t* allocator) {
  arena->allocator = allocator;
  arena->size = size;
  arena->base = (stdr_uptr)allocator->new (size);
  STDR_ASSERT(arena->base != 0);
  arena->pos = arena->base;
}

void stdr_arena_new_static(stdr_arena_t* arena, void* buf,
                           stdr_usize buf_size) {
  arena->allocator = NULL;
  arena->size = buf_size;
  arena->base = (stdr_uptr)buf;
  arena->pos = arena->base;
}

void* stdr_arena_alloc_new(stdr_arena_t* arena, stdr_usize size) {
  stdr_uptr ptr = arena->pos;
  arena->pos += size;
  STDR_ASSERT(arena->pos + size <= arena->base + arena->size);
  return (void*)ptr;
}

void stdr_arena_alloc_delete(stdr_arena_t* arena, void* ptr) {
  (void)arena;
  (void)ptr;
}

void stdr_arena_delete(stdr_arena_t* arena) {
  if (arena->allocator != NULL) {
    arena->allocator->delete ((void*)arena->base);
  }

  arena->base = 0;
  arena->size = 0;
  arena->pos = 0;
}

void stdr_arena_reset(stdr_arena_t* arena) { arena->pos = arena->base; }

void stdr_arena_ctx_init(stdr_usize size, const stdr_allocator_t* allocator) {
  stdr_arena_init(&STDR_ARENA_CTX, size, allocator);
}

void stdr_arena_ctx_init_static(void* buf, stdr_usize buf_size) {
  stdr_arena_new_static(&STDR_ARENA_CTX, buf, buf_size);
}

void* stdr_arena_ctx_alloc_new(stdr_usize size) {
  return stdr_arena_alloc_new(&STDR_ARENA_CTX, size);
}

void stdr_arena_ctx_alloc_delete(void* ptr) {
  stdr_arena_alloc_delete(&STDR_ARENA_CTX, ptr);
}

void stdr_arena_ctx_delete(void) { stdr_arena_delete(&STDR_ARENA_CTX); }

// * String

stdr_str_t stdr_read_entire_file(stdr_arena_t* arena,
                                 const stdr_cstr_t filename) {
  stdr_dstr_t dstr;
  stdr_dstr_init(&dstr, &STDR_ALLOCATOR_LIBC);

  FILE* f = fopen(filename, "r");
  if (f == NULL) {
    printf("[ERROR] Could not read: '%s'\n", filename);
  }

  STDR_ASSERT(f != NULL);

  while (STDR_TRUE) {
    int ch = getc(f);
    if (ch == EOF) break;
    stdr_dstr_append(&dstr, (char)ch);
  }

  fclose(f);

  stdr_str_t str = stdr_dstr_to_str(arena, dstr);
  stdr_dstr_delete(&dstr);
  return str;
}

stdr_str_t stdr_str_concat(stdr_arena_t* arena, stdr_str_t a, stdr_str_t b) {
  char* sptr = (char*)stdr_arena_alloc_new(arena, a.size + b.size);
  memcpy(sptr, a.ptr, a.size);
  memcpy(sptr + a.size, b.ptr, b.size);
  return (stdr_str_t){.ptr = sptr, .size = a.size + b.size};
}

stdr_str_t stdr_str_concat_all_impl(stdr_arena_t* arena, stdr_str_t a, ...) {
  va_list args;
  va_start(args, a);

  stdr_str_t b = va_arg(args, stdr_str_t);
  while (b.ptr != NULL) {
    a = stdr_str_concat(arena, a, b);
    b = va_arg(args, stdr_str_t);
  }

  va_end(args);

  return a;
}

stdr_str_t stdr_cstr_to_str(stdr_arena_t* arena, const stdr_cstr_t cstr) {
  stdr_usize n = strlen(cstr);
  void* dst = stdr_arena_alloc_new(arena, n);
  memcpy(dst, cstr, n);
  return (stdr_str_t){.ptr = dst, .size = n};
}

char* stdr_str_to_cstr(stdr_arena_t* arena, stdr_str_t str) {
  char* dst = stdr_arena_alloc_new(arena, str.size + 1);
  memcpy((void*)dst, str.ptr, str.size);
  dst[str.size] = '\0';
  return dst;
}

stdr_str_t stdr_str_retain(stdr_arena_t* arena, stdr_str_t a,
                           stdr_bool (*f)(char)) {
  char* sptr = (char*)stdr_arena_alloc_new(arena, a.size);
  stdr_str_t ret = (stdr_str_t){.ptr = sptr, .size = 0};
  for (stdr_usize i = 0; i < a.size; i++) {
    if (f(a.ptr[i])) {
      sptr[ret.size++] = a.ptr[i];
    }
  }
  return ret;
}

void stdr_str_split_at(stdr_str_t a, stdr_usize mid, stdr_str_t* out1,
                       stdr_str_t* out2) {
  STDR_ASSERT(mid <= a.size);

  out1->ptr = a.ptr;
  out1->size = mid;

  out2->ptr = a.ptr + mid;
  out2->size = a.size - mid;
}

stdr_bool stdr_str_split_once(stdr_str_t a, stdr_str_t pattern,
                              stdr_str_t* out1, stdr_str_t* out2) {
  stdr_usize i;
  stdr_bool ok = stdr_str_find(a, pattern, &i);
  if (!ok) return STDR_FALSE;

  if (out1 != NULL) {
    out1->ptr = a.ptr;
    out1->size = i;
  }

  if (out2 != NULL) {
    out2->ptr = a.ptr + i + pattern.size;
    out2->size = a.size - i - pattern.size;
  }

  return STDR_TRUE;
}

stdr_bool stdr_str_contains(stdr_str_t a, stdr_str_t pattern) {
  return stdr_str_find(a, pattern, NULL);
}

stdr_bool stdr_str_starts_with(stdr_str_t a, stdr_str_t pattern) {
  if (a.size < pattern.size) return FALSE;
  a.size = pattern.size;
  return stdr_str_eq(a, pattern);
}

stdr_bool stdr_str_find(stdr_str_t a, stdr_str_t pattern,
                        stdr_usize* out_index) {
  if (pattern.size > a.size) return STDR_FALSE;

  for (stdr_usize i = 0; i <= a.size - pattern.size; i++) {
    if (memcmp(&a.ptr[i], pattern.ptr, pattern.size) == 0) {
      if (out_index != NULL) *out_index = i;
      return STDR_TRUE;
    };
  }

  return STDR_FALSE;
}

stdr_i64 stdr_str_parse_i64(stdr_str_t str) {
  stdr_i64 n = 0;

  stdr_i64 sign = 1;
  if (str.ptr[0] == '-') sign = -1;
  if (str.ptr[0] == '+') str.ptr++;

  for (usize i = 0; i < str.size; i++) {
    n *= 10;
    STDR_ASSERT(str.ptr[i] >= '0' && str.ptr[i] <= '9');
    n += str.ptr[i] - '0';
  }

  return n * sign;
}

stdr_bool stdr_str_eq(stdr_str_t a, stdr_str_t b) {
  if (a.size != b.size) return STDR_FALSE;

  return memcmp(a.ptr, b.ptr, a.size) == 0;
}

stdr_str_t stdr_str_drop_begin(stdr_str_t str, stdr_usize count) {
  STDR_ASSERT(str.size >= count);
  str.ptr += count;
  str.size -= count;
  return str;
}

stdr_str_t stdr_str_drop_end(stdr_str_t str, stdr_usize count) {
  STDR_ASSERT(str.size >= count);
  str.size -= count;
  return str;
}

void stdr_dstr_init(stdr_dstr_t* dstr, const stdr_allocator_t* allocator) {
  dstr->allocator = allocator;
  dstr->ptr = NULL;
  dstr->size = 0;
  dstr->capacity = 0;
}
void stdr_dstr_append(stdr_dstr_t* dstr, char ch) {
  STDR_ASSERT(dstr->allocator != NULL);

  if (dstr->size >= dstr->capacity) {
    dstr->capacity = (dstr->capacity == 0)
                         ? 4
                         : (stdr_usize)((stdr_f64)dstr->capacity * 1.5);
    char* ptr_new = dstr->allocator->new (dstr->capacity);
    if (dstr->ptr != NULL) memcpy(ptr_new, dstr->ptr, dstr->size);
    dstr->allocator->delete (dstr->ptr);
    dstr->ptr = ptr_new;
  }

  dstr->ptr[dstr->size++] = ch;
}

void stdr_dstr_append_str(stdr_dstr_t* dstr, stdr_str_t str) {
  for (usize i = 0; i < str.size; i++) {
    stdr_dstr_append(dstr, str.ptr[i]);
  }
}
stdr_str_t stdr_dstr_to_str(stdr_arena_t* arena, const stdr_dstr_t dstr) {
  STDR_ASSERT(arena->allocator != NULL);

  if (dstr.size == 0) return (stdr_str_t){.ptr = NULL, .size = 0};

  char* ptr = stdr_arena_alloc_new(arena, dstr.size);
  memcpy(ptr, dstr.ptr, dstr.size);
  return (stdr_str_t){.ptr = ptr, .size = dstr.size};
}

void stdr_dstr_delete(stdr_dstr_t* dstr) {
  dstr->allocator->delete (dstr->ptr);
}

// cmd and build

stdr_bool stdr_build_is_current(stdr_str_t infile, stdr_str_t outfile) {
  stdr_arena_t arena;
  stdr_arena_init(&arena, 1024, &STDR_ALLOCATOR_LIBC);

  const char* cinfile = stdr_str_to_cstr(&arena, infile);
  const char* coutfile = stdr_str_to_cstr(&arena, outfile);

  struct stat statbuf = {0};
  if (stat(coutfile, &statbuf) < 0) {
    stdr_arena_delete(&arena);
    return STDR_FALSE;
  }

  stdr_i64 output_path_time = statbuf.st_mtime;

  if (stat(cinfile, &statbuf) < 0) {
    stdr_arena_delete(&arena);
    STDR_ASSERT(STDR_FALSE && "UNREACHABLE");
  }

  stdr_i64 input_path_time = statbuf.st_mtime;

  if (input_path_time <= output_path_time) {
    stdr_arena_delete(&arena);
    return STDR_TRUE;
  };

  stdr_arena_delete(&arena);
  return STDR_FALSE;
}

void stdr_cmd_delete(const stdr_cmd_t* cmd) { stdr_array_delete(&cmd->cmd); }

void stdr_cmd_append(stdr_cmd_t* cmd, stdr_str_t str) {
  if (cmd->cmd == NULL) {
    cmd->cmd = stdr_array_new(stdr_str_t, &STDR_ALLOCATOR_LIBC);
  }

  stdr_array_append(&cmd->cmd, str);
}

void stdr_cmd_append_all_impl(stdr_cmd_t* cmd, stdr_str_t str, ...) {
  va_list args;
  va_start(args, str);

  while (str.ptr != NULL) {
    stdr_cmd_append(cmd, str);
    str = va_arg(args, stdr_str_t);
  }

  va_end(args);
}

void stdr_cmd_reset(stdr_cmd_t* cmd) { stdr_array_reset(cmd->cmd); }

void stdr_cmd_log(stdr_cmd_t* cmd, const stdr_writable_t* writable) {
  stdr_writable_fmt(writable, S("[CMD] "), 0);
  for (usize i = 0; i < stdr_array_count(cmd->cmd); i++) {
    stdr_writable_fmt(writable, S("%s "), cmd->cmd[i]);
  }
  stdr_writable_fmt(writable, S("\n"), 0);
}

void stdr_cmd_append_cc(stdr_cmd_t* cmd) {
  stdr_str_t a, b;
  if (stdr_str_split_once(S(STDR_CC), S(" "), &a, &b)) {
    stdr_cmd_append_all(cmd, a, b);
  } else {
    stdr_cmd_append_all(cmd, S(STDR_CC));
  }
}

void stdr_build_self(stdr_str_t filename, int argc, char** argv) {
  stdr_arena_t arena;
  stdr_arena_init(&arena, 1024, &STDR_ALLOCATOR_LIBC);

  stdr_str_t binary = stdr_cstr_to_str(&arena, argv[0]);
  if (stdr_build_is_current(filename, binary)) {
    stdr_arena_delete(&arena);
    return;
  }
  stdr_str_t binary_old = stdr_str_concat(&arena, binary, S(".old"));

  stdr_cmd_t cmd = {0};
  stdr_cmd_append_all(&cmd, S("mv"), binary, binary_old);
  stdr_cmd_run(&cmd);
  stdr_cmd_reset(&cmd);

  stdr_cmd_append_cc(&cmd);

  stdr_str_t dcc = S("-DCC=\"" STDR_CC "\"");
  stdr_cmd_append_all(&cmd, dcc, filename, S("-o"), binary, S("-Iinc"));
  stdr_cmd_run(&cmd);
  stdr_cmd_reset(&cmd);

  stdr_cmd_append_all(&cmd, binary);
  for (usize i = 1; i < (usize)argc; i++) {
    stdr_cmd_append(&cmd, stdr_cstr_to_str(&arena, argv[i]));
  }
  stdr_cmd_run(&cmd);
  stdr_cmd_reset(&cmd);

  stdr_cmd_delete(&cmd);
  stdr_arena_delete(&arena);

  exit(0);
}

void stdr_cmd_run_reset(stdr_cmd_t* cmd) {
  stdr_cmd_run(cmd);
  stdr_cmd_reset(cmd);
}

void stdr_cmd_run(stdr_cmd_t* cmd) {
  stdr_arena_t arena;
  stdr_arena_init(&arena, 1024 * 4, &STDR_ALLOCATOR_LIBC);

  stdr_cmd_log(cmd, &STDR_WRITABLE_STDOUT);

  const char* proc = stdr_str_to_cstr(&arena, cmd->cmd[0]);

  stdr_array(char*) args = stdr_array_new(char*, &STDR_ALLOCATOR_LIBC);

  for (usize i = 0; i < stdr_array_count(cmd->cmd); i++) {
    char* cstr = stdr_str_to_cstr(&arena, cmd->cmd[i]);
    stdr_array_append(&args, cstr);
  }
  stdr_array_append(&args, NULL);

  pid_t pid = fork();
  if (pid == 0) {
    execvp(proc, args);
  } else {
    int stat;
    waitpid(pid, &stat, 0);
    if (!WIFEXITED(stat)) {
      printf("[CMD] %s\n", strsignal(WTERMSIG(stat)));
    }
  }
  stdr_array_delete(&args);
  stdr_arena_delete(&arena);
}

#endif
#undef STDR_IMPLEMENTATION
