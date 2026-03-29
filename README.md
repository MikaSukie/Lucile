# Lucile

Lucile is a C-like language with structs, enums, generics, contextual typing, a compile-time crumb checker, and LLVM IR output. Imports are merged into the current compilation unit, and duplicate imports are ignored. Diagnostics preserve file paths for imported sources when available. 

## Overview

Lucile is compiled as a whole unit. The backend emits LLVM IR text. Target datalayout and target triple may be supplied through directives.

## Comments

- Single-line comments use `//`
- Block comments use `/* ... */`
- Nested block comments are supported

## Literals

Supported literals:

- Integer literals
- Floating point literals
- String literals
- Character literals
- `true`
- `false`

String literals that contain `{` are treated as format strings by the compiler. Escape sequences are supported in strings and characters, including `\n`, `\t`, `\r`, `\\`, `\"`, `\'`, `\0`, `\{`, and `\}`. 

## Types

Built-in types:

- `void`
- `bool`
- `i8`, `i16`, `i32`, `i64`
- `u8`, `u16`, `u32`, `u64`
- `f32`, `f64`
- `str`
- `char`
- `int`
- `float`

Compound types:

- Pointer types use a prefix `*`, such as `*i64`
- Arrays use suffix syntax, such as `i64[5]`
- Struct and enum types use their declared names
- Generic type arguments use angle brackets, such as `Result<str, i32>`

`int` and `float` are generic numeric types that may be resolved by the compiler to concrete runtime types during lowering. `#` is the contextual type placeholder.

## Variables

Variables may be declared at global scope or inside functions.

```
i64 count = 0;
str message = "hello";
bool ready = true;
i64[5] values;
```

Uninitialized values are zero-initialized by the compiler. Global variables may omit an initializer. `nomd` marks a binding as non-modifiable.

```
nomd i64 x = 1;
i64 nomd y = 2;
```

## Functions

Basic form:

```
name(type1 a, type2 b) return_type {
    return 0;
}
```

A function may also use angle-bracket return syntax in contextual cases, for example `name(...) <#> { ... }`. The parser accepts both `ret` and `return` as the return keyword. 

A function may fall through without an explicit return. The compiler handles the missing return during lowering when appropriate. 

### Extern functions

Extern declarations end with `;`.

```
extern printf(str fmt,) i32;
```

A trailing comma before `)` marks the function as variadic, and this is only accepted for extern declarations. `unsafe` may prefix an extern declaration.

### Main entry point

A function named `main` is the default entry point. The compiler requires `main` unless `@nomain;` is present.

### Special parameters

`argv` and `argc` are recognized as special parameter forms.

```
main(argv args, argc count) i32 {
    return 0;
}
```

`argv` is treated as a pointer to string-like values, and `argc` is treated as `i32`.

## Structs

```
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
```

`packed struct` is supported. Struct fields are declared with a type followed by a name and a semicolon. `nomd` may appear on fields.

Struct initialization uses named fields and order is independent.

```
Rectangle r = Rectangle { x(1); y(2); width(3); height(4); };
```

## Enums

```
enum Exist<T> {
    None;
    Some(T);
}
```

Enums may carry payloads on variants. Generic enums may have multiple generic parameters. Enum construction uses `EnumName->Variant(...)`.

## Expressions

Supported expression forms include:

- Binary operators
- Unary operators
- Calls
- Casts
- Indexing
- Field access
- Ternary expressions
- Array literals
- Struct initializers
- Enum construction
- String interpolation literals

### Operators

Arithmetic:

- `+ - * / %`

Assignment:

- `= += -= *= /= %=`
- `&= |= ^= <<= >>=`

Comparison:

- `== != < > <= >=`

Logical:

- `&& || !`

Bitwise:

- `& | ^ ~ << >>`

Special forms:

- `*` for dereference and pointer types
- `&` for address-of
- `.` for field access
- `->` for enum variant construction
- `? :` for ternary expressions

### Casting

C-style casting is supported:

```
i64 x = (i64)3.14;
```

## Arrays

Fixed-size arrays are supported.

```
i64[5] arr;
arr[1] = 2;

i64[2] d = [5, 8];
```

## Control flow

### If

```
if (cond) {
    ...
} else {
    ...
}
```

### While

```
while (cond) {
}
```

### Loop

```
loop {
}
```

Infinite loop form:

```
loop(counter, cond, step) {
}
```

The first slot may be a declaration or an expression. `br` and `cont` are supported for break and continue. `ret` is also accepted as a synonym for `return`.

### Return

```
return expr;
```

A bare `return;` is valid where appropriate.

## Pattern matching

### Match

```
match (value) {
    Some(x) {
        ...
    }
    None {
        ...
    }
}
```

### Typeswitch

```
typeswitch(#) {
    typecase(i64) {
        ...
    }
    typecase(str) {
        ...
    }
    fallback {
        ...
    }
}
```

The subject may be `#` or an identifier. `#` refers to the contextual type. `fallback` is optional. The compiler errors if no matching typecase is found and no fallback exists for a concrete type.

## Generics

```
tostring(T candidate) str {
    typeswitch(T) {
        typecase(i64) {
            return i64tostr(candidate);
        }
        typecase(f64) {
            return ftostr(candidate);
        }
        fallback {
            return "tostring error";
        }
    }
}
```

The compiler specializes generic functions when they are used. Generated specializations use the reserved `__LUCILE__` prefix. `__LUCILE__` is reserved for compiler-generated names.

Generic type parameters may also appear in enum and type positions. The parser accepts generic parameter names in function parameter lists and generic type arguments after identifiers.

## Contextual return typing

```
input(str s) # {
    typeswitch(#) {
        typecase(i64)  { return iinput(s); }
        typecase(f64)  { return finput(s); }
        typecase(f32)  { return f32input(s); }
        typecase(bool) { return binput(s); }
        typecase(str)  { return sinput(s); }
    }
}
```

## Imports

```
import file;
import a, b, c;
import "path/file.lc";
```

Identifier imports are resolved as module names, string imports are resolved relative to the current source file. Duplicate imports are ignored, and recursive imports are allowed.

## Compiler directives

```
@nomain;
@tdl="...";
@ttriple="...";
@cwarn;
```

- `@nomain;` disables the requirement for a `main` function
- `@tdl="...";` sets the LLVM target datalayout
- `@ttriple="...";` sets the LLVM target triple
- `@cwarn;` enables crumb diagnostics as warnings instead of hard errors

## Crumb system

```
crumble(x)!r=0!w=0;
dropall(x, y, z);
readonly(x);
writeonly(x);
transfer(src, dst);
```

`crumble` controls read and write limits. Missing `!r` or `!w` means unlimited, and the compiler warns about omitted limits. The checker tracks heap-backed values conservatively. The current compiler treats crumb diagnostics as errors by default unless `@cwarn;` is enabled. The `--no-crumb` flag skips the checker entirely.

## Diagnostics

Compiler diagnostics include file path, line number, a caret marker, and `~` underlines for multi-character spans. Warnings and errors use the same source location style.

## Backend

Lucile compiles to LLVM IR.
