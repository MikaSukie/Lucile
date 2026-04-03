# Lucile Language Specifications

Lucile is a strict language whose compiler's main job is mainly to just produce LLVM IR, specifically well LLVM 17+ output for LLVM 20 compat.
The compiler should support an AST dump arg, and optionally a lex dump. The whole purpose is just outputting LLVM IR.

---

## 1. Core Syntax

### Function declarations

```
funcname(type name, type name) type {
    //
}
```

```
funcname(type name, type name) # {
    //
}
```

The language must support multi parameter generics / full generics.

Mangle rules are `__Lucile__funcname_type_type_etc`, where the type mangle adds with params and is predictable.

`__LUCILE__` is a reserved name, and the user must not be allowed to use the prefix.

---

## 2. Compile-Time Typeswitch and Monomorphisation

Typeswitch is all done at compile time. Monomorphisation is as well. Do not duplicate emission. Also must support DCE for unused branches for types.

```
funcname(type name, T name) type {
    typeswitch(param /*generic like T*/) {
        typecase(type) { /*do action*/ }
        // fallback branch "fallback {/*do*/}"
    }
    // return if above cases fall thru as fallback alternative. must be exhaustive.
}
```

```
funcname(type name, type name) # {
    typeswitch(#) {
        typecase(type) { /*do action*/ }
    }
    // # is the contextual return type symbol. having a fallback in this case is unsafe. disallowed.
    // you may monomorphise with the __LUCILE__name_ctx_type_etc as an example. CTX for context.
}
```

One last addition to typeswitch, both `#` and `[T]`, is the `in` (insert) functionality. Basically it is an extension/insert of a typeswitch branch.

```
in func_to_insert_to {
    typecase(int) for (#) { // can use # or any local parameter name that EXISTS in that function. if it doesnt, must error.
        return iinput(s);
    }
    // typecase(int)   { return iinput(s); }
}
```

Inserts must be ALL collected for functions THEN evaluated during monomorphisation or usage, to prevent weird ordering bugs.

If a typecase of the same type already exists, error saying so, what like.

---

## 3. Errors and Diagnostics

The error system requirements are strict: ALL errors, and every error, should use one unified system.

Diagnostics must:

- show which line
- point to the correct file across imports
- show the line number
- show the offending line
- below it, show with a `^` the offending token
- if the token is longer than 1, use `^~~~~~` style where `~` is added for the rest of the token length

Errors must be proper.

This is a strict language. Wrong syntax errors must exist.

---

## 4. Imports

Import system just inlines into the main file, then compiles.

Used as:

```
import nameoffile;
import name, name;
```

It auto inserts the `.lc` if it doesn't exist, `.slc`.

If the same name with `.lc` and `.slc` exists, ask for an explicit import.

Explicit import works like this:

```
import "file.ext", "file.ext", name;
```

You can mix and match name and explicit.

Explicit imports should allow directory imports:

```
import "dir/dir2/file.ext";
```

Recursive imports may exist, so import each file ONE time and ignore future occurrences.

---

## 5. Variables and Globals

Var declaration goes:

```
type name;
type name = val;
nomd type name = val;
nomd type name;
// zero initialized value.
```

`nomd` = non mutable.

Globals are just variables that are defined with the same grammar, just global scope. Name collision errors must exist. `nomd` globals are just constants, so simple.

---

## 6. Builtin Types

Should support the following 3 datatypes.

Language builtin type names:

- `i8`, `i16`, `i32`, `i64`
- `u8`, `u16`, `u32`, `u64` (proper zext sext)
- `bool`
- `char`
- `str`
- `f32`, `f64` (floats)
- `void` (func returns)

---

## 7. Structs

Full structs (regular) and packed structs (C compat.) must be supported.

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

---

## 8. Arrays

Should have normal array support.

```
type[maxindexnum] name;
// optionally
type[maxindexnum] name = [initial, values];
```

---

## 9. Enums and Tagged Unions

Full tagged unions must be supported.

```
enum Exist<T> {
  None;
  Some(T);
}

enum Respond<T, E> {
    Ok(T);
    Err(E);
}

enum Color { Red; Green; Blue; }
enum Opt { None; Some(int); }
```

Both generic and regular tagged unions must be supported.

Enum and struct field access rules:

- struct field access uses the `.` operator. Example: `Structname.field`
- enum field access uses the `->` operator. Example: `Enumname->variant`

---

## 10. Operators and Control Flow

Ternary support `? :` is required.

Also match case pattern matching must exist.

```
test_safe_div_direct() i32 {
    Respond<int, string> r = safe_div_direct(10, 2);
    match (r) {
        Ok(v) {
            printnl("quotient: ");
            print(itostr(v)); newline();
        }
        Err(msg) {
            print("error: "); print(msg); newline();
        }
    }
    return 0;
}
```

Regular while loops are required. `if cond {}` and `while cond {}` do not require parentheses `()` if used; parentheses are for precedence.

`break;` and `continue;` support is required.

---

## 11. Zero Initialization and Return Behavior

This is a zero-initialized language.

BUT do NOT override the user's init value if it exists.

Disallow self referencing before definition.

Return values are not required for a function that returns any type; must return the zero initializer for that type.

---

## 12. Type System and Casting

The type system supports user custom types.

Well, functions can return user custom types: enums and structs. That is it. Builtin types and enum and structs.

C style casting should be added:

```
(type) val
```

Nested casts if necessary.

Math operations should support the same C operators:

- `~`
- `/`
- `*`
- `%`
- `&`
- `|`
- `^`
- `>>`
- `<<`

Compound assignments are required too.

Math operators must have proper precedence like C.

`*` is for pointers, `&` is for address of.

Just use `+` for string concatenation. It is a strictly typed language so only string string case would work anyway, or numbers numbers.

---

## 13. Compiler Directives

Compiler directives:

```
@tdl="";
@ttriple="";
@nomain;
```

- `@tdl=""` = target data layout
- `@ttriple=""` = target triple
- `@nomain` = the compiler should tell the user if there is no main function, or to use `nomain` to silence it, for FFI out/library use

Target data layout and target triple should not be output by the compiler by itself. LLVM overrides it automatically anyway.

The above directives are optional, but just included in case needed.

---

## 14. Main Entry, argc, argv

`argc` and `argv` must work in this fashion:

```
main(argc anylocalnamethatisntaglobal, argv anylocalnamethatisntaglobal) {}
```

Yes, `argv` is an array as per standard.

---

## 15. Example Program

A example that should work:

```
extern printf(str fmt,) void;

main() i32 {
    i32 f = 8;
    f = f + f;
    f += f;
    printf("hello %d %d %d\n", f, f, f);
    Exist<str> stro = Exist->Some(input("file path?: "));
    print(unwrap_exist(stro) + "\n");
    print(unwrap_res_to_str(try_read_file(stro)) + "\n");
    f32 g = 0.0;
}

extern exit(i32 code) void;

panic(str reason) void {
    print("Panicked because: " + reason + "\n");
    exit(1);
}

unwrap_exist(Exist<str> s) str {
    match (s) {
        Some(t) {
            return t;
        }
        None {
            panic("Nothing Inputted");
        }
    }
}

unwrap_res_to_str(Respond<str, str> s) str {
    match (s) {
        Ok(d) {
            return d;
        }
        Err(e) {
            panic(e);
        }
    }
}

extern print(str s) void;
extern read_file(str path) str;
extern file_exists(str path) bool;

enum Exist<T> {
  None;
  Some(T);
}

enum Respond<T, E> { 
    Ok(T);
    Err(E);
}

try_read_file(Exist<str> path) Respond<str, str> {
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

extern sinput(str input) str;
extern iinput(str input) i32;
extern binput(str input) bool;
extern finput(str input) f64;

input(str s) # {
    typeswitch(#) {
        typecase(i32)   { return iinput(s); }
        typecase(f64) { return finput(s); }
        typecase(bool)  { return binput(s); }
        typecase(str){ return sinput(s); }
    }
}
```

---

## 16. Crumb System and Memory Ownership Rules

This system accompanies a robust scope autodrop.

# Ownership & Access Semantics Specification

## 16.1 Overview

This language defines a safe pointer / ownership model based on:

- Readable handles (`readonly`)
- Exclusive mutable handles (`take`)
- State transition via release (`release`)
- Automatic destruction (scope / last use)
- Optional usage constraints (`crumble`)

The system guarantees:

- No stale reads
- No concurrent mutation + reading
- Deterministic memory safety without lifetimes

---

## 16.2 Core Concepts

### 16.2.1 Value vs Handle

A value is the underlying storage. A handle is a reference to that value.

Handles can be:

- Readable (R)
- Mutable (M)
- Invalid (⊥)

### 16.2.2 States

Each value is always in exactly one of:

- `Readable` (R-state): zero or more readable handles exist
- `Mutable` (M-state): exactly one mutable handle exists
- `Dropped` (D-state): value is destroyed

---

## 16.3 Operations

### 16.3.1 `readonly(x)`

Creates a readable handle to value `x`.

Rules:

- Allowed only if value is in `Readable` state
- Produces a new readable handle `r`
- Unlimited readable handles may exist

Example:

```
r1 = readonly(x)
r2 = readonly(x)   // allowed
```

### 16.3.2 `take(x)`

Creates the only mutable handle to value `x`.

Rules:

- Allowed only if no active mutable handle exists
- Transitions value to `Mutable` state
- Produces mutable handle `m`

Effects:

- ALL existing readable handles are invalidated permanently
- No new readable handles may be created until `release(x)`

Example:

```
r = readonly(x)
m = take(x)

use(r)   // ERROR: r is invalidated
```

### 16.3.3 `release(x)`

Ends mutation and restores readability.

Rules:

- Must be called on the active mutable handle
- Transitions value from `Mutable` -> `Readable`
- Produces a new readable handle

Effects:

- Allows creation of new readers again
- Does NOT restore old readers

Example:

```
m = take(x)
r_new = release(x)

r_old = readonly(x) before take
use(r_old)  // ERROR (still invalid)
```

### 16.3.4 `crumble(x)!r=n!w=n`

Optional usage constraint system.

Behavior:

- Tracks read count (`r`)
- Tracks write count (`w`)

Compile-time:

- Error if exceeded
- Warning if underused

Notes:

- Does NOT affect safety
- Purely a constraint / hint system for the programmer/user

---

## 16.4 Invalidation Rules

### 16.4.1 Mutation invalidation

When `take(x)` occurs:

- All readable handles become Invalid (⊥)
- Invalid handles:
  - Cannot be used
  - Cannot be revived
  - Cause compile-time errors if accessed

### 16.4.2 Permanent invalidation

Invalidation is irreversible.

```
r = readonly(x)
take(x)
release(x)

use(r) // ERROR
```

---

## 16.5 Drop Semantics

### 16.5.1 Automatic Drop

Values are dropped when:

- Last valid handle is used (last-use semantics), OR
- End of scope

### 16.5.2 Invalid handles

- Do NOT count as valid usage
- Do NOT prevent drop

---

## 16.6 Edge Cases and Detailed Behavior

### 16.6.1 Use-after-invalidation

```
r = readonly(x)
take(x)

use(r) // ERROR: invalidated handle
```

### 16.6.2 Reader creation during mutation

```
take(x)
readonly(x) // ERROR: cannot create readers during mutation
```

### 16.6.3 Multiple mutators

```
take(x)
take(x) // ERROR: mutator already exists either delete this mutator, or release the other first.
```

### 16.6.4 Aliasing

```
a = x
b = x

take(a)
use(b) // ERROR: alias invalidated
```

Rule:

All aliases share the same underlying state.

### 16.6.5 Function calls (mutation inside function)

```
fn mutate(v) {
    take(v)
}

r = readonly(x)
mutate_func(x)

use(r) // ERROR
```

Rule:

- `take` inside any function invalidates all external readers

### 16.6.6 Function returning readable after mutation

```
fn mutate(v) {
    m = take(v)
    return release(v)
}

r1 = readonly(x)
r2 = mutate(x)

use(r1) // ERROR
use(r2) // OK
```

### 16.6.7 Conditional mutation

```
r = readonly(x)

if cond {
    take(x)
}

use(r) // ERROR (conservative analysis)
```

Rule:

- If mutation may occur, readers are considered invalid

### 16.6.8 Loops

```
r = readonly(x)

loop {
    take(x)
    release(x)
}

use(r) // ERROR
```

Rule:

- First mutation invalidates `r`
- Remains invalid forever

### 16.6.9 Rebinding / reassignment

```
r = readonly(x)
r = readonly(x) // new handle

take(x)

use(r) // ERROR
```

Rule:

- Handles track the underlying value, not variable identity

### 16.6.10 Temporary reads

```
foo(readonly(x))
take(x) // OK
```

Rule:

- Temporary (non-stored) readers expire immediately after expression

### 16.6.11 Nested mutation attempt

```
m1 = take(x)
m2 = take(x) // ERROR
```

### 16.6.12 Mutation without release

```
m = take(x)
// no release

end_scope
```

Rule:

- Compiler must either auto-release before drop OR error if value cannot safely transition to drop

### 16.6.13 Using invalid handle in crumble context

```
r = readonly(x)
take(x)

crumble(r)!r=1 // ERROR: r invalid
```

### 16.6.14 Interleaved readers and mutation attempt

```
r1 = readonly(x)
r2 = readonly(x)

take(x)

use(r1) // ERROR
use(r2) // ERROR
```

### 16.6.15 Attempt to recreate reader without release

```
take(x)
readonly(x) // ERROR
```

### 16.6.16 Multiple releases

```
m = take(x)
r = release(x)
release(x) // ERROR
```

---

## 16.7 Summary of Guarantees

This system guarantees:

1. No read observes mutated data without re-acquisition
2. Mutation is always exclusive
3. All stale references are statically rejected
4. Memory is automatically managed via scope and last-use
5. No lifetime annotations required

---

## 16.8 Design Principle

Mutation resets the validity of all prior observations. Only the mutator defines the next valid readable state.
