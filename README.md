# Lucile
Basically BhumiLang if it was in C and also better.

# Lucile Language Spec

## Overview

Lucile has been inspired by both my Bhumi and Orcat languages.
They are also inspired by, python, C, rust

---

## Entry Point

    main() i32 {
        return 0;
    }

Functions do not strictly need to return their declared type.  
If no return is provided, the zero initializer of the return type is used.

---

## Comments

- Single line comments use `//`
- Block comments use `/* ... */`
- Nested block comments are supported

---

## Variables (Global Scope)

    i64 f;
    str g;
    str hello = "hello!";
    bool TRUE = true;

    Color pink = Color { r(255); g(50); b(255); a(128); }

- Globals may be uninitialized
- Uninitialized values are zero initialized

---

## Functions

### Basic function

    test(str f) void {
    }

### External functions

    extern printf(str fmt,) void;

A leading comma indicates variadic arguments. Only valid for extern declarations.

---

## Arrays

    i64[5] arr;
    arr[1] = 2;

    i64[2] d = [5, 8];

- Fixed size arrays
- Inline initialization supported

---

## Type system notes

- Everything is typed, including expressions
- Type inference is allowed where possible
- Default initialization is zero initialization

---

## Contextual return types

    input(str s) # {
        typeswitch(#) {
            typecase(i64)  { return iinput(s); }
            typecase(f64)  { return finput(s); }
            typecase(f32)  { return f32input(s); }
            typecase(bool) { return binput(s); }
            typecase(str)  { return sinput(s); }
        }
    }

- `#` represents the expected left hand side type
- Enables contextual polymorphism

---

## Generics

    tostring(T candidate) str {
        typeswitch(T) {
            typecase(i64) {
                return i64tostr(candidate);
            }
            typecase(f64) {
                return ftostr(candidate);
            }
            typecase(f32) {
                return f32tostr(candidate);
            }
            typecase(bool) {
                return btostr(candidate);
            }
            typecase(str) {
                return tostr(candidate);
            }
        }

        return "tostring error: no matching typecase found";
    }

### Notes

- Multi parameter generics supported
- Mangling uses __LUCILE__ prefix
- __LUCILE__ is reserved

---

## Exhaustiveness rules

All typeswitch statements must be exhaustive.

Optional fallback:

    typeswitch(T) {
        typecase(i64) { }
        typecase(f64) { }
        fallback { return "error"; }
    }

Missing cases are compile errors.

---

## Enums

    enum Exist<T> {
        None;
        Some(T);
    }

    enum Respond<T, E> {
        Ok(T);
        Err(E);
    }

Enum access uses:

    EnumName->Field

---

## Pattern matching

    try_read_file(Exist<str> path) <Respond<str, str>> {
        match (path) {
            Some(p) {
                if (file_exists(p)) {
                    return Respond->Ok(read_file(p));
                } else {
                    return Respond->Err("Error reading file");
                }
            }
            None {
                return Respond->Err("No path given");
            }
        }
    }

---

## Enum usage

    main() void {
        Exist<str> thepath = Exist->Some(input("file path?: "));

        match (thepath) {
            Some(contents) {
                printnl("path is: " + contents);
            }
            None {
                printnl("Null err");
            }
        }

        Respond<str, str> infile = try_read_file(thepath);

        match (infile) {
            Ok(contents) {
                printnl(contents);
            }
            Err(e) {
                printnl(e);
            }
        }
    }

---

## Ternary operator

    Toggle t = x ? Toggle->On : Toggle->Off;

---

## Structs

    struct Rectangle {
        f32 x;
        f32 y;
        f32 width;
        f32 height;
    }

    packed struct Color {
        u8 r;
        u8 g;
        u8 b;
        u8 a;
    }

---

## Operators

### Arithmetic
    + - * / %

### Assignment
    = += -= *= /= %=

### Bitwise
    & | ^ ~ << >>

### Comparison
    == != > < >= <=

### Logical
    && || !

### Special
    *   pointer
    &   address of
    .   field access
    ->  enum field access

---

## Casting

    (type) expression

C style casting with standard precedence rules.

---

## Strings and interpolation

    "{varname} text {more}"

- Interpolation replaces braces
- Escape sequences support \{ and \}

---

## Struct initialization

    Player p = Player { health(100); ammo(30); }

- Named fields
- Order independent

---

## Crumb system

The crumb system controls read and write permissions and ownership cleanup.

### Syntax

    crumble(var)!r=0!w=0;

- r is read count
- w is write count
- omitted values mean unlimited (compiler warns)

### Macros

    dropall(x, y, z);
    readonly(x, y, z);
    writeonly(x, y, z);

### Rules

- Multiple readers allowed
- Only one active writer allowed
- Writing revokes prior references
- Stale references must be revalidated
- A crumbled value becomes unusable after `dropall(...)`
- When a crumbled value is unusable and its lifetime ends, the compiler emits cleanup/free for the owned value
- Overwriting an owned pointer value also releases the prior contents before the new value is stored
- `return`, `break`, `continue`, and normal scope exit must trigger cleanup for live owned crumbled values
- Compiler tracks usage and enforces limits

### Required usage targets

Crumb declarations are required for:

- Heap strings from extern functions
- Structs and enums when mutable
- Raw pointers and references
- Closures with captures
- Mutable variables

### Optional usage

- Stack locals
- Scalars
- Non mutable variables

### Drop behavior

- Scope based automatic cleanup
---

## Mutability

    nomd

Marks a value as non modifiable.

---

## Typecase extension syntax

    in tostring {
        typecase(int8) for (candidate) {
            return i8tostr(candidate);
        }
    }

- External typecase injection
- Must remain exhaustive

---

## Main arguments

    main(argv a, argc b) i32 {
        return 0;
    }

- Argument order does not matter
- Unused arguments may be omitted

---

## Compiler directives

    @nomain;
    @ttriple="";
    @tdl="";

Disables requirement for main,
Sets the target triple,
Sets the target data layout.

---

## Error rules

Compiler errors must include:

- Line numbers
- Caret indicators
- Multi character highlights using ~
- Suggested fixes when possible

---

## Loops

loop constructs:

    loop(counter, condition, step) {
    }

    loop {} // same as while(true) {}

    while (cond) {}
- br and cont supported

---

## Imports

    import file;
    import a, b, c;
    import "path/file.ext";

- Duplicate imports ignored (recursive imports allowed)
- Files merged into one unit

---

## Types

- Signed: i8 i16 i32 i64
- Unsigned: u8 u16 u32 u64
- Floats: f32 f64
- Other: bool str char
- Compound: struct enum
- Pointers supported

---

## Unsafe functions

    unsafe extern func() rettype;

- May return stack or heap memory
- Lifetime not managed by compiler

---

## Additional rules

- Array bounds checking required
- Hex literals supported
- Struct fields order independent
- Pointer arithmetic allowed but safety aware

---

## Backend

Compiles to LLVM IR
