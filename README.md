# Advent of Code 2024

## Run all tests
Bootstrap the build script (It will rebuild itself after the first compile if any edits are detected). Define CC as an compiler argument to define your compiler of choice ("cc" is the default).
```console
cc build.c -o build # -DCC="\"clang\""
./build
```

## Documentation

Documentation is not complete. An incomplete documentation is also available via the [testfile](tests/test.c). Just read the source.

### STDR As a build script 
 ```c
#ifndef CC // Must be defined before "stdr.h"
#define CC "cc"
#endif

#define STDR_IMPLEMENTATION
#include "stdr.h"

int main(void) {
  STDR_BUILD_SELF(argc, argv); // Will rebuild rerun itself after changes in the script.

  stdr_cmd_t cmd = {0};
  stdr_cmd_append_cc(&cmd); // Defined with CC definition
  stdr_cmd_append_all(&cmd, S("proc.c"), S("-o"), S("proc"));
  stdr_cmd_run_reset(&cmd);
  stdr_cmd_delete(&cmd);
}
```

### STDR array
 ```c
#define STDR_IMPLEMENTATION
#include "stdr.h"

int main(void) {
  array(i64) xs = stdr_array_new(i64, &ALLOCATOR_LIBC);
  array_append(&xs, 7);
  array_append(&xs, 8);
  array_append(&xs, 7);
  forr (i64, x, xs) {
    stdr_stdout_fmt(S("%i, "), *x);
  }
  stdr_stdout_fmt(S("\n"), NULL);
  array_delete(&xs)
}
```

### STDR str
```c
#define STDR_IMPLEMENTATION
#include "stdr.h"

int main(void) {
  stdr_arena_t arena = {0};
  stdr_arena_init(&arena, 1024, &STDR_ALLOCATOR_LIBC);

  str_t c = stdr_str_concat(&arena, S("Hello, "), S("World!"));
  STDR_ASSERT(stdr_str_eq(c, S("Hello, World!")));
}
```

### STDR regex
```c
#define STDR_IMPLEMENTATION // Implementation must be provided for stdr_regex.h
#include "stdr.h"
#define STDR_REGEX_IMPLEMENTATION
#include "stdr_regex.h"

int main(void) {
  regex_t regex = {0};
  regex_state_t s0 = regex_push_state(&regex); // begin-state
  regex_compile(&regex, s0, S("mul(\\d+,\\d+)")); // Note: parenthesis are character literals
  
  str_t tstr = S("mul(112,2)+some junk");
  str_t match = regex_match_str_start(&regex,  s0, tstr, tstr.ptr);
  STDR_ASSERT(stdr_str_eq(match, S("mul(112,2)")));
  
  tstr = S("notmul(112,2)+some junk");
  match = regex_match_str_start(&regex,  s0, tstr, tstr.ptr);
  STDR_ASSERT(match.ptr == NULL);

  regex_delete(&regex);
}
```

