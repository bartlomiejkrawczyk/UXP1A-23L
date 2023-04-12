# Style
## Naming
### Functions
C-compatible functions are `snake_case`. C++-only functions should be distinguishable from those
by using <!-- TODO: Decide? -->.
### Structs
C-compatible structs are named using the `snake_case` style, with an accompanying `typedef`
appending `_t` to their name.
```c
typedef struct example {
    int x;
} example_t;

// usage
struct example a;
example_t b;
```
Structs and classes using C++ only features (types, templates, inheritance, references, methods)
should be `PascalCase` to distinguish them from C ones.
### Headers
Headers that are C-only should use the `.h` extension, C++ headers the `.hpp` one.
## File structure
### Libraries
`libfs-core` and `libfs` are libraries, and as such have `include` directories separate from
their `src`.
### Executables
`libfs-daemon` and any clients are executables not meant to be used as libraries, so they lack
separate `include` directories. Any header files should be alongside the `.c` or `.cpp` files
in the `src` directory.
## Formatting
Formatting should be handled with `clang-format`, fitting the `.clang-format` file in the root
of the repo.
