#ifndef LUCILE_H
#define LUCILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>

/* ================================================================
   ARENA ALLOCATOR
   ================================================================ */

#define ARENA_BLOCK(1 << 20)

typedef struct ArenaBlock {
    char * data;
    size_t used, cap;
    struct ArenaBlock * next;
}
ArenaBlock;
typedef struct Arena {
    ArenaBlock * head;
}
Arena;

Arena * arena_new(void);
void * arena_alloc(Arena * a, size_t sz);
char * arena_strdup(Arena * a,
    const char * s);
char * arena_strndup(Arena * a,
    const char * s, int n);
char * arena_sprintf(Arena * a,
    const char * fmt, ...);
void arena_free(Arena * a);

/* ================================================================
   TOKENS
   ================================================================ */

typedef enum {

    TOK_INT_LIT,
    TOK_FLOAT_LIT,
    TOK_STRING_LIT,
    TOK_CHAR_LIT,
    TOK_TRUE,
    TOK_FALSE,

    TOK_IDENT,

    TOK_RET,
    TOK_IF,
    TOK_ELSE,
    TOK_LOOP,
    TOK_STRUCT,
    TOK_PACKED,
    TOK_ENUM,
    TOK_MATCH,
    TOK_EXTERN,
    TOK_UNSAFE,
    TOK_IMPORT,
    TOK_IN,
    TOK_TYPESWITCH,
    TOK_TYPECASE,
    TOK_FALLBACK,
    TOK_NOMD,
    TOK_CRUMBLE,
    TOK_DROPALL,
    TOK_READONLY,
    TOK_WRITEONLY,
    TOK_BR,
    TOK_CONT,
    TOK_ARGV,
    TOK_ARGC,

    TOK_I8,
    TOK_I16,
    TOK_I32,
    TOK_I64,
    TOK_U8,
    TOK_U16,
    TOK_U32,
    TOK_U64,
    TOK_F32,
    TOK_F64,
    TOK_STRING_KW,
    TOK_CHAR_KW,
    TOK_BOOL_KW,
    TOK_VOID_KW,
    TOK_INT_KW,
    TOK_FLOAT_KW,

    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_LBRACKET,
    TOK_RBRACKET,
    TOK_SEMICOLON,
    TOK_COMMA,
    TOK_DOT,
    TOK_ARROW,
    TOK_HASH,
    TOK_AT,
    TOK_QUESTION,
    TOK_COLON,
    TOK_LANGLE,
    TOK_RANGLE,
    TOK_BANG_CRUMB,

    TOK_PLUS,
    TOK_MINUS,
    TOK_STAR,
    TOK_SLASH,
    TOK_PERCENT,

    TOK_EQ,
    TOK_PLUSEQ,
    TOK_MINUSEQ,
    TOK_STAREQ,
    TOK_SLASHEQ,
    TOK_PERCENTEQ,
    TOK_AMPEQ,
    TOK_PIPEEQ,
    TOK_CARETEQ,
    TOK_LSHIFTEQ,
    TOK_RSHIFTEQ,

    TOK_EQEQ,
    TOK_NEQ,
    TOK_GTE,
    TOK_LTE,

    TOK_ANDAND,
    TOK_OROR,
    TOK_BANG,

    TOK_AMP,
    TOK_PIPE,
    TOK_CARET,
    TOK_TILDE,
    TOK_LSHIFT,
    TOK_RSHIFT,

    TOK_NOMAIN_DIR,
    TOK_EOF,
    TOK_ERROR
}
TokenType;

typedef struct {
    TokenType type;
    int line, col;
    const char * start;
    int length;
    union {
        int64_t ival;
        double fval;
        char * sval;
    };
}
Token;

/* ================================================================
   TYPE SYSTEM
   ================================================================ */

typedef enum {
    TY_VOID,
    TY_BOOL,
    TY_I8,
    TY_I16,
    TY_I32,
    TY_I64,
    TY_U8,
    TY_U16,
    TY_U32,
    TY_U64,
    TY_F32,
    TY_F64,
    TY_STRING,
    TY_CHAR,
    TY_ARRAY,
    TY_PTR,
    TY_STRUCT,
    TY_ENUM,
    TY_FUNC,
    TY_GENERIC,
    TY_CONTEXT,
    TY_INT_GENERIC,
    TY_FLOAT_GENERIC,
}
TypeKind;

typedef struct Type {
    TypeKind kind;
    union {
        struct {
            struct Type * elem;
            int64_t count;
        }
        array;
        struct {
            struct Type * pointee;
        }
        ptr;
        struct {
            const char * name;
        }
        named;
        struct {
            const char * param;
        }
        generic;
        struct {
            struct Type ** params;
            int param_count;
            struct Type * ret;
            bool variadic;
        }
        func;
    };
}
Type;

/* ================================================================
   AST NODE KINDS
   ================================================================ */

typedef enum {

    ND_PROGRAM,
    ND_FUNC_DECL,
    ND_STRUCT_DECL,
    ND_ENUM_DECL,
    ND_GLOBAL_VAR,
    ND_EXTERN_DECL,
    ND_IMPORT,
    ND_NOMAIN,
    ND_IN_EXT,

    ND_BLOCK,
    ND_VAR_DECL,
    ND_ASSIGN,
    ND_RETURN,
    ND_IF,
    ND_LOOP,
    ND_MATCH,
    ND_MATCH_ARM,
    ND_TYPESWITCH,
    ND_TYPECASE,
    ND_CRUMBLE,
    ND_BREAK,
    ND_CONTINUE,
    ND_EXPR_STMT,

    ND_BINARY,
    ND_UNARY,
    ND_CAST,
    ND_CALL,
    ND_INDEX,
    ND_FIELD,
    ND_ENUM_FIELD,
    ND_TERNARY,
    ND_IDENT,
    ND_INT_LIT,
    ND_FLOAT_LIT,
    ND_STRING_LIT,
    ND_CHAR_LIT,
    ND_BOOL_LIT,
    ND_ARRAY_LIT,
    ND_STRUCT_INIT,
    ND_FORMAT_STR,
    ND_HASH,
}
NodeKind;

typedef struct ASTNode {
    NodeKind kind;
    int line;
    const char * source_path;
    Type * ty;
    union {

        struct {
            struct ASTNode ** items;
            int count;
        }
        program;

        struct {
            const char * name;
            const char ** param_names;
            Type ** param_types;
            bool * param_nomd;
            int param_count;
            bool variadic;
            Type * ret_type;
            struct ASTNode * body;
            bool is_extern;
            bool is_unsafe;
            const char * generic_param;
        }
        func;

        struct {
            const char * name;
            const char ** field_names;
            Type ** field_types;
            bool * field_nomd;
            int field_count;
            bool packed;
        }
        strct;

        struct {
            const char * name;
            const char * generic_param;
            const char ** variant_names;
            Type ** variant_types;
            int variant_count;
        }
        enm;

        struct {
            const char * name;
            Type * type;
            struct ASTNode * init;
            bool nomd;
        }
        var;

        struct {
            const char ** paths;
            int count;
        }
        import;

        struct {
            const char * func_name;
            Type * tc_type;
            const char * tc_param;
            struct ASTNode * tc_body;
        }
        in_ext;

        struct {
            struct ASTNode ** stmts;
            int count;
        }
        block;

        struct {
            struct ASTNode * lhs, * rhs;
            TokenType op;
        }
        assign;

        struct {
            struct ASTNode * val;
        }
        ret;

        struct {
            struct ASTNode * cond, * then_body, * else_body;
        }
        ifstmt;

        struct {
            struct ASTNode * counter;
            struct ASTNode * cond;
            struct ASTNode * step;
            struct ASTNode * body;
        }
        loop;

        struct {
            struct ASTNode * subject;
            struct ASTNode ** arms;
            int arm_count;
        }
        match;

        struct {
            const char * variant;
            const char * bind_name;
            struct ASTNode * body;
        }
        match_arm;

        struct {
            const char * subject_name;
            struct ASTNode ** cases;
            int case_count;
            struct ASTNode * fallback;
        }
        typeswitch;

        struct {
            Type * match_type;
            struct ASTNode * body;
        }
        typecase;

        struct {
            const char ** vars;
            int var_count;
            int read_limit;
            int write_limit;
            bool is_dropall;
            bool is_readonly;
            bool is_writeonly;
        }
        crumble;

        struct {
            TokenType op;
            struct ASTNode * lhs, * rhs;
        }
        binary;

        struct {
            TokenType op;
            struct ASTNode * operand;
        }
        unary;

        struct {
            Type * target;
            struct ASTNode * operand;
        }
        cast;

        struct {
            struct ASTNode * callee;
            struct ASTNode ** args;
            int arg_count;
            Type * explicit_type;
        }
        call;

        struct {
            struct ASTNode * array, * index;
        }
        idx;

        struct {
            struct ASTNode * object;
            const char * field;
        }
        field;

        struct {
            const char * enum_name;
            const char * variant;
            struct ASTNode ** args;
            int arg_count;
        }
        enum_field;

        struct {
            struct ASTNode * cond, * then_val, * else_val;
        }
        ternary;

        struct {
            const char * name;
        }
        ident;
        const char * file;

        struct {
            int64_t val;
        }
        int_lit;
        struct {
            double val;
        }
        float_lit;
        struct {
            const char * val;
        }
        str_lit;
        struct {
            char val;
        }
        char_lit;
        struct {
            bool val;
        }
        bool_lit;

        struct {
            struct ASTNode ** elems;
            int count;
        }
        array_lit;

        struct {
            const char * struct_name;
            const char ** field_names;
            struct ASTNode ** field_vals;
            int field_count;
        }
        struct_init;

        struct {
            const char * raw;
        }
        fmt_str;

        struct {
            struct ASTNode * expr;
        }
        expr_stmt;
    };
}
ASTNode;

/* ================================================================
   LEXER
   ================================================================ */

typedef struct {
    const char * src;
    int pos, line, col;
    Arena * arena;
    Token peeked;
    bool has_peeked;
}
Lexer;

Lexer * lexer_new(Arena * a,
    const char * src);
Token lexer_next(Lexer * l);
Token lexer_peek(Lexer * l);
void lexer_consume(Lexer * l);
const char * token_type_name(TokenType t);

/* ================================================================
   PARSER
   ================================================================ */

typedef struct {
    Lexer * lexer;
    Arena * arena;
    Token cur, prev;
    bool had_error;
    bool panic;
}
Parser;

Parser * parser_new(Arena * a, Lexer * l);
ASTNode * parse_program(Parser * p);

/* ================================================================
   CRUMB CHECKER (compile-time ownership enforcement)
   ================================================================ */

void crumb_check(ASTNode * program);
void semantic_check(ASTNode * program);

/* ================================================================
   CODE GENERATOR  (emits LLVM IR text)
   ================================================================ */

typedef struct SymEntry {
    const char * name;
    Type * type;
    const char * llvm_name;
    bool is_global;
    bool is_enum_tag;
    int enum_index;
    struct SymEntry * next;
}
SymEntry;

typedef struct SymScope {
    SymEntry * entries;
    struct SymScope * parent;
}
SymScope;

typedef struct GenericSpec {
    const char * key;
    const char * name;
    const char * generic_name;
    Type * actual;
    ASTNode * fn;
    ASTNode * spec;
    bool emitted;
    struct GenericSpec * next;
}
GenericSpec;

typedef struct {
    FILE * out;
    Arena * arena;
    ASTNode * program;
    int tmp;
    int lbl;
    SymScope * scope;

    const char ** str_pool;
    int str_count, str_cap;

    const char * cur_func;
    Type * cur_ret;
    bool ret_emitted;
    bool suppress_warn;
    const char * target_datalayout;
    const char * target_triple;
    const char * generic_name;
    Type * generic_actual;
    Type * context_actual;
    GenericSpec * specs;

    ASTNode ** struct_decls;
    int struct_count;
    ASTNode ** enum_decls;
    int enum_count;
    ASTNode ** func_decls;
    int func_count;

    ASTNode ** in_exts;
    int in_ext_count;
    bool nomain;
}
Codegen;

void codegen_run(FILE * out, ASTNode * program, Arena * a);

/* ================================================================
   UTILITIES
   ================================================================ */

void lc_fatal(const char * fmt, ...);
void lc_error(int line, int col,
    const char * fmt, ...);
void lc_error_tok(int line, int col, int toklen,
    const char * fmt, ...);
void lc_error_at(const char * path, int line, int col,
    const char * fmt, ...);
void lc_error_tok_at(const char * path, int line, int col, int toklen,
    const char * fmt, ...);
void lc_warn(int line,
    const char * fmt, ...);

ASTNode * make_node(Arena * a, NodeKind k, int line);
Type * make_type(Arena * a, TypeKind k);

const char * type_to_llvm(Type * t);
const char * type_to_llvm_elem(Type * t);
bool type_is_integer(Type * t);
bool type_is_signed(Type * t);
bool type_is_float(Type * t);
bool types_equal(Type * a, Type * b);
Type * resolve_int_type(void);
Type * resolve_float_type(void);

extern Arena * g_arena;

extern int g_errors;

extern
const char * g_source;
extern
const char * g_source_path;

#endif

#include <unistd.h>

/* ================================================================
   GLOBALS
   ================================================================ */

Arena * g_arena = NULL;
int g_errors = 0;
const char * g_source = NULL;
const char * g_source_path = NULL;
static
const char * g_target_datalayout = NULL;
static
const char * g_target_triple = NULL;

static char * g_imported_paths[1024];
static int g_imported_count = 0;

static
const char * g_diag_paths[1024];
static
const char * g_diag_sources[1024];
static int g_diag_count = 0;

static void register_source_file(const char * path,
    const char * src) {
    if (!path || !src) return;
    for (int i = 0; i < g_diag_count; i++) {
        if (strcmp(g_diag_paths[i], path) == 0) {
            g_diag_sources[i] = src;
            return;
        }
    }
    if (g_diag_count < 1024) {
        g_diag_paths[g_diag_count] = path;
        g_diag_sources[g_diag_count] = src;
        g_diag_count++;
    }
}

static void diag_use_source_path(const char * path) {
    if (!path) return;
    for (int i = 0; i < g_diag_count; i++) {
        if (strcmp(g_diag_paths[i], path) == 0) {
            g_source = g_diag_sources[i];
            g_source_path = g_diag_paths[i];
            return;
        }
    }
}

/* ================================================================
   FILE I/O
   ================================================================ */

static char * read_file(const char * path) {
    FILE * f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "error: cannot open '%s'\n", path);
        exit(1);
    }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    rewind(f);
    char * buf = malloc((size_t) sz + 1);
    if (!buf) {
        fprintf(stderr, "error: out of memory\n");
        exit(1);
    }
    size_t r = fread(buf, 1, (size_t) sz, f);
    buf[r] = '\0';
    fclose(f);
    return buf;
}

/* ================================================================
   USAGE / HELP
   ================================================================ */

static void usage(const char * prog) {
    fprintf(stderr,
        "Lucile compiler  v0.1\n"
        "usage: %s [options] <source.lc>\n"
        "options:\n"
        "  -o <file>     output file (default: out.ll)\n"
        "  --dump-ast    print AST to stderr\n"
        "  --no-crumb    skip crumb checker\n"
        "  -h, --help    show this message\n",
        prog);
    exit(0);
}

/* ================================================================
   SIMPLE AST DUMP (for --dump-ast)
   ================================================================ */

static
const char * node_kind_name(NodeKind k) {
    switch (k) {
    case ND_PROGRAM:
        return "PROGRAM";
    case ND_FUNC_DECL:
        return "FUNC_DECL";
    case ND_STRUCT_DECL:
        return "STRUCT_DECL";
    case ND_ENUM_DECL:
        return "ENUM_DECL";
    case ND_GLOBAL_VAR:
        return "GLOBAL_VAR";
    case ND_EXTERN_DECL:
        return "EXTERN_DECL";
    case ND_IMPORT:
        return "IMPORT";
    case ND_NOMAIN:
        return "NOMAIN";
    case ND_IN_EXT:
        return "IN_EXT";
    case ND_BLOCK:
        return "BLOCK";
    case ND_VAR_DECL:
        return "VAR_DECL";
    case ND_ASSIGN:
        return "ASSIGN";
    case ND_RETURN:
        return "RETURN";
    case ND_IF:
        return "IF";
    case ND_LOOP:
        return "LOOP";
    case ND_MATCH:
        return "MATCH";
    case ND_MATCH_ARM:
        return "MATCH_ARM";
    case ND_TYPESWITCH:
        return "TYPESWITCH";
    case ND_TYPECASE:
        return "TYPECASE";
    case ND_CRUMBLE:
        return "CRUMBLE";
    case ND_BREAK:
        return "BREAK";
    case ND_CONTINUE:
        return "CONTINUE";
    case ND_EXPR_STMT:
        return "EXPR_STMT";
    case ND_BINARY:
        return "BINARY";
    case ND_UNARY:
        return "UNARY";
    case ND_CAST:
        return "CAST";
    case ND_CALL:
        return "CALL";
    case ND_INDEX:
        return "INDEX";
    case ND_FIELD:
        return "FIELD";
    case ND_ENUM_FIELD:
        return "ENUM_FIELD";
    case ND_TERNARY:
        return "TERNARY";
    case ND_IDENT:
        return "IDENT";
    case ND_INT_LIT:
        return "INT_LIT";
    case ND_FLOAT_LIT:
        return "FLOAT_LIT";
    case ND_STRING_LIT:
        return "STRING_LIT";
    case ND_CHAR_LIT:
        return "CHAR_LIT";
    case ND_BOOL_LIT:
        return "BOOL_LIT";
    case ND_ARRAY_LIT:
        return "ARRAY_LIT";
    case ND_STRUCT_INIT:
        return "STRUCT_INIT";
    case ND_FORMAT_STR:
        return "FORMAT_STR";
    case ND_HASH:
        return "HASH";
    default:
        return "???";
    }
}

static void dump_indent(int d) {
    for (int i = 0; i < d; i++) fputs("  ", stderr);
}

static void dump_ast(ASTNode * n, int depth) {
    if (!n) return;
    dump_indent(depth);
    fprintf(stderr, "[%s] (line %d)", node_kind_name(n -> kind), n -> line);

    switch (n -> kind) {
    case ND_FUNC_DECL:
    case ND_EXTERN_DECL:
        fprintf(stderr, " name=%s", n -> func.name);
        break;
    case ND_STRUCT_DECL:
        fprintf(stderr, " name=%s", n -> strct.name);
        break;
    case ND_ENUM_DECL:
        fprintf(stderr, " name=%s", n -> enm.name);
        break;
    case ND_GLOBAL_VAR:
    case ND_VAR_DECL:
        fprintf(stderr, " name=%s", n ->
            var.name);
        break;
    case ND_IDENT:
        fprintf(stderr, " '%s'", n -> ident.name);
        break;
    case ND_INT_LIT:
        fprintf(stderr, " %lld", (long long) n -> int_lit.val);
        break;
    case ND_FLOAT_LIT:
        fprintf(stderr, " %g", n -> float_lit.val);
        break;
    case ND_STRING_LIT:
        fprintf(stderr, " \"%s\"", n -> str_lit.val);
        break;
    case ND_BOOL_LIT:
        fprintf(stderr, " %s", n -> bool_lit.val ? "true" : "false");
        break;
    case ND_BINARY:
        fprintf(stderr, " op=%s", token_type_name(n -> binary.op));
        break;
    case ND_UNARY:
        fprintf(stderr, " op=%s", token_type_name(n -> unary.op));
        break;
    default:
        break;
    }
    fputc('\n', stderr);

    switch (n -> kind) {
    case ND_PROGRAM:
        for (int i = 0; i < n -> program.count; i++)
            dump_ast(n -> program.items[i], depth + 1);
        break;
    case ND_FUNC_DECL:
    case ND_EXTERN_DECL:
        dump_ast(n -> func.body, depth + 1);
        break;
    case ND_BLOCK:
        for (int i = 0; i < n -> block.count; i++)
            dump_ast(n -> block.stmts[i], depth + 1);
        break;
    case ND_IF:
        dump_ast(n -> ifstmt.cond, depth + 1);
        dump_ast(n -> ifstmt.then_body, depth + 1);
        dump_ast(n -> ifstmt.else_body, depth + 1);
        break;
    case ND_RETURN:
        dump_ast(n -> ret.val, depth + 1);
        break;
    case ND_BINARY:
        dump_ast(n -> binary.lhs, depth + 1);
        dump_ast(n -> binary.rhs, depth + 1);
        break;
    case ND_UNARY:
        dump_ast(n -> unary.operand, depth + 1);
        break;
    case ND_CALL:
        dump_ast(n -> call.callee, depth + 1);
        for (int i = 0; i < n -> call.arg_count; i++)
            dump_ast(n -> call.args[i], depth + 1);
        break;
    case ND_VAR_DECL:
    case ND_GLOBAL_VAR:
        dump_ast(n ->
            var.init, depth + 1);
        break;
    case ND_ASSIGN:
        dump_ast(n -> assign.lhs, depth + 1);
        dump_ast(n -> assign.rhs, depth + 1);
        break;
    case ND_MATCH:
        dump_ast(n -> match.subject, depth + 1);
        for (int i = 0; i < n -> match.arm_count; i++)
            dump_ast(n -> match.arms[i], depth + 1);
        break;
    case ND_MATCH_ARM:
        dump_ast(n -> match_arm.body, depth + 1);
        break;
    case ND_LOOP:
        dump_ast(n -> loop.counter, depth + 1);
        dump_ast(n -> loop.cond, depth + 1);
        dump_ast(n -> loop.step, depth + 1);
        dump_ast(n -> loop.body, depth + 1);
        break;
    case ND_EXPR_STMT:
        dump_ast(n -> expr_stmt.expr, depth + 1);
        break;
    case ND_TERNARY:
        dump_ast(n -> ternary.cond, depth + 1);
        dump_ast(n -> ternary.then_val, depth + 1);
        dump_ast(n -> ternary.else_val, depth + 1);
        break;
    case ND_INDEX:
        dump_ast(n -> idx.array, depth + 1);
        dump_ast(n -> idx.index, depth + 1);
        break;
    case ND_FIELD:
        dump_ast(n -> field.object, depth + 1);
        break;
    case ND_TYPESWITCH:
        for (int i = 0; i < n -> typeswitch.case_count; i++)
            dump_ast(n -> typeswitch.cases[i], depth + 1);
        dump_ast(n -> typeswitch.fallback, depth + 1);
        break;
    case ND_TYPECASE:
        dump_ast(n -> typecase.body, depth + 1);
        break;
    default:
        break;
    }
}

/* ================================================================
   MAIN
   ================================================================ */

int main(int argc, char ** argv) {
    const char * input_path = NULL;
    const char * output_path = "out.ll";
    bool dump_ast_flag = false;
    bool no_crumb = false;

    if (argc < 2) usage(argv[0]);

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) usage(argv[0]);
        else if (strcmp(argv[i], "--dump-ast") == 0) dump_ast_flag = true;
        else if (strcmp(argv[i], "--no-crumb") == 0) no_crumb = true;
        else if (strcmp(argv[i], "-o") == 0) {
            if (++i >= argc) {
                fprintf(stderr, "error: -o requires filename\n");
                exit(1);
            }
            output_path = argv[i];
        } else if (argv[i][0] == '-') {
            fprintf(stderr, "error: unknown option '%s'\n", argv[i]);
            exit(1);
        } else {
            if (input_path) {
                fprintf(stderr, "error: multiple input files\n");
                exit(1);
            }
            input_path = argv[i];
        }
    }

    if (!input_path) {
        fprintf(stderr, "error: no input file\n");
        exit(1);
    }

    g_target_datalayout = NULL;
    g_target_triple = NULL;

    char * source = read_file(input_path);
    register_source_file(input_path, source);
    g_source = source;
    g_source_path = input_path;

    g_imported_paths[g_imported_count++] = (char * ) input_path;
    register_source_file(input_path, source);

    g_arena = arena_new();

    Lexer * lexer = lexer_new(g_arena, source);

    Parser * parser = parser_new(g_arena, lexer);
    ASTNode * ast = parse_program(parser);

    if (parser -> had_error) {
        arena_free(g_arena);
        free(source);
        return 1;
    }

    semantic_check(ast);
    if (g_errors > 0) {
        arena_free(g_arena);
        free(source);
        return 1;
    }

    if (dump_ast_flag) dump_ast(ast, 0);

    if (!no_crumb) {
        crumb_check(ast);
        if (g_errors > 0) {
            arena_free(g_arena);
            free(source);
            return 1;
        }
    }

    char tmp_path[4096];
    snprintf(tmp_path, sizeof(tmp_path), "%s.tmp%d", output_path, (int) getpid());

    FILE * out = fopen(tmp_path, "w");
    if (!out) {
        fprintf(stderr, "error: cannot open output '%s'\n", output_path);
        arena_free(g_arena);
        free(source);
        return 1;
    }

    codegen_run(out, ast, g_arena);
    fclose(out);

    if (g_errors > 0) {
        remove(tmp_path);
        arena_free(g_arena);
        free(source);
        return 1;
    }

    if (rename(tmp_path, output_path) != 0) {
        fprintf(stderr, "error: cannot write output '%s'\n", output_path);
        remove(tmp_path);
        arena_free(g_arena);
        free(source);
        return 1;
    }

    fprintf(stderr, "OK  -> %s\n", output_path);
    arena_free(g_arena);
    free(source);
    return 0;
}

/* ================================================================
   ARENA
   ================================================================ */

Arena * arena_new(void) {
    Arena * a = malloc(sizeof(Arena));
    a -> head = NULL;
    return a;
}

static ArenaBlock * block_new(size_t min_cap) {
    size_t cap = min_cap > ARENA_BLOCK ? min_cap : ARENA_BLOCK;
    ArenaBlock * b = malloc(sizeof(ArenaBlock));
    b -> data = malloc(cap);
    b -> used = 0;
    b -> cap = cap;
    b -> next = NULL;
    return b;
}

void * arena_alloc(Arena * a, size_t sz) {

    sz = (sz + 7) & ~(size_t) 7;
    if (!a -> head || a -> head -> used + sz > a -> head -> cap) {
        ArenaBlock * b = block_new(sz);
        b -> next = a -> head;
        a -> head = b;
    }
    void * p = a -> head -> data + a -> head -> used;
    a -> head -> used += sz;
    memset(p, 0, sz);
    return p;
}

char * arena_strdup(Arena * a,
    const char * s) {
    if (!s) return NULL;
    size_t n = strlen(s) + 1;
    char * p = arena_alloc(a, n);
    memcpy(p, s, n);
    return p;
}

char * arena_strndup(Arena * a,
    const char * s, int n) {
    char * p = arena_alloc(a, (size_t) n + 1);
    memcpy(p, s, (size_t) n);
    p[n] = '\0';
    return p;
}

char * arena_sprintf(Arena * a,
    const char * fmt, ...) {
    char buf[4096];
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    return arena_strndup(a, buf, n);
}

void arena_free(Arena * a) {
    ArenaBlock * b = a -> head;
    while (b) {
        ArenaBlock * next = b -> next;
        free(b -> data);
        free(b);
        b = next;
    }
    free(a);
}

/* ================================================================
   UTILITIES
   ================================================================ */

static void print_source_line(const char * path, int line_num, int col, int token_len) {
    if (!g_source || line_num <= 0) return;

    const char * p = g_source;
    for (int i = 1; i < line_num && * p; i++) {
        while ( * p && * p != '\n') p++;
        if ( * p == '\n') p++;
    }
    if (! * p) return;

    const char * end = p;
    while ( * end && * end != '\n') end++;

    int indent = 0;
    const char * trimmed = p;
    while (trimmed < end && ( * trimmed == ' ' || * trimmed == '\t')) {
        indent++;
        trimmed++;
    }

    if (path && * path) fprintf(stderr, "%s:%d| %.*s\n", path, line_num, (int)(end - p), p);
    else fprintf(stderr, "%d| %.*s\n", line_num, (int)(end - p), p);

    if (col <= 0) col = indent + 1;
    char lnum_buf[64];
    int prefix_len = path && * path ?
        snprintf(lnum_buf, sizeof(lnum_buf), "%s:%d| ", path, line_num) :
        snprintf(lnum_buf, sizeof(lnum_buf), "%d| ", line_num);
    for (int i = 0; i < prefix_len + col - 1; i++) fprintf(stderr, " ");
    fprintf(stderr, "^");
    int squig = (token_len > 1) ? token_len - 1 : 0;
    for (int i = 0; i < squig; i++) fprintf(stderr, "~");
    fprintf(stderr, "\n");

    (void) trimmed;
}

static int source_first_content_col(int line_num) {
    if (!g_source || line_num <= 0) return 1;
    const char * p = g_source;
    for (int i = 1; i < line_num && * p; i++) {
        while ( * p && * p != 10) p++;
        if ( * p == 10) p++;
    }
    if (! * p) return 1;
    int col = 1;
    while ( * p == ' ' || * p == 9) {
        p++;
        col++;
    }
    return col;
}

static int source_content_len(int line_num) {
    if (!g_source || line_num <= 0) return 1;
    const char * p = g_source;
    for (int i = 1; i < line_num && * p; i++) {
        while ( * p && * p != 10) p++;
        if ( * p == 10) p++;
    }
    if (! * p) return 1;
    const char * end = p;
    while ( * end && * end != 10) end++;
    while (p < end && ( * p == ' ' || * p == 9)) p++;
    int len = (int)(end - p);
    return len > 0 ? len : 1;
}

static int source_first_content_col_at(const char * path, int line_num) {
    diag_use_source_path(path);
    return source_first_content_col(line_num);
}

static int source_content_len_at(const char * path, int line_num) {
    diag_use_source_path(path);
    return source_content_len(line_num);
}

static int source_ident_col_at(const char * path, int line_num,
    const char * ident) {
    diag_use_source_path(path);
    if (!g_source || !ident || ! * ident) return source_first_content_col(line_num);
    const char * p = g_source;
    for (int i = 1; i < line_num && * p; i++) {
        while ( * p && * p != 10) p++;
        if ( * p == 10) p++;
    }
    if (! * p) return source_first_content_col(line_num);
    const char * end = p;
    while ( * end && * end != 10) end++;
    const char * hit = strstr(p, ident);
    while (hit && hit < end) {
        bool left_ok =
            (hit == p) ||
            (!isalnum((unsigned char) hit[-1]) && hit[-1] != '_');

        char r = hit[strlen(ident)];
        bool right_ok =
            (r == '\0') ||
            (!isalnum((unsigned char) r) && r != '_');

        if (left_ok && right_ok) {
            int col = 1;
            for (const char * q = p; q < hit; q++) col++;
            return col;
        }
        hit = strstr(hit + 1, ident);
    }
    return source_first_content_col(line_num);
}

void lc_fatal(const char * fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "Fatal: ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    exit(1);
}

void lc_error(int line, int col,
    const char * fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "Error: ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    print_source_line(g_source_path, line, col, 1);
    g_errors++;
}

void lc_error_tok(int line, int col, int toklen,
    const char * fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "Error: ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    if (toklen < 1) toklen = 1;
    print_source_line(g_source_path, line, col, toklen);
    g_errors++;
}

void lc_error_at(const char * path, int line, int col,
    const char * fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    diag_use_source_path(path);
    fprintf(stderr, "Error: ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    print_source_line(path, line, col, 1);
    g_errors++;
}

void lc_error_tok_at(const char * path, int line, int col, int toklen,
    const char * fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    diag_use_source_path(path);
    fprintf(stderr, "Error: ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    if (toklen < 1) toklen = 1;
    print_source_line(path, line, col, toklen);
    g_errors++;
}

void lc_warn(int line,
    const char * fmt, ...) {
    (void) line;
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "Warning: ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
}

/* ================================================================
   AST / TYPE CONSTRUCTORS
   ================================================================ */

ASTNode * make_node(Arena * a, NodeKind k, int line) {
    ASTNode * n = arena_alloc(a, sizeof(ASTNode));
    n -> kind = k;
    n -> line = line;
    n -> source_path = g_source_path;
    return n;
}

Type * make_type(Arena * a, TypeKind k) {
    Type * t = arena_alloc(a, sizeof(Type));
    t -> kind = k;
    return t;
}

/* ================================================================
   TYPE HELPERS
   ================================================================ */

static Type * _i64 = NULL;
static Type * _f64 = NULL;

Type * resolve_int_type(void) {
    if (!_i64) {
        _i64 = make_type(g_arena, TY_I64);
    }

    return _i64;
}

Type * resolve_float_type(void) {
    if (!_f64) {
        _f64 = make_type(g_arena, TY_F64);
    }
    return _f64;
}

bool type_is_integer(Type * t) {
    if (!t) return false;
    switch (t -> kind) {
    case TY_BOOL:
    case TY_I8:
    case TY_I16:
    case TY_I32:
    case TY_I64:
    case TY_U8:
    case TY_U16:
    case TY_U32:
    case TY_U64:
    case TY_CHAR:
    case TY_INT_GENERIC:
        return true;
    default:
        return false;
    }
}

bool type_is_signed(Type * t) {
    if (!t) return false;
    switch (t -> kind) {
    case TY_I8:
    case TY_I16:
    case TY_I32:
    case TY_I64:
    case TY_INT_GENERIC:
        return true;
    default:
        return false;
    }
}

bool type_is_float(Type * t) {
    if (!t) return false;
    return t -> kind == TY_F32 || t -> kind == TY_F64 || t -> kind == TY_FLOAT_GENERIC;
}

bool types_equal(Type * a, Type * b) {
    if (!a || !b) return a == b;
    if (a -> kind != b -> kind) return false;
    switch (a -> kind) {
    case TY_ARRAY:
        return a -> array.count == b -> array.count && types_equal(a -> array.elem, b -> array.elem);
    case TY_PTR:
        return types_equal(a -> ptr.pointee, b -> ptr.pointee);
    case TY_STRUCT:
    case TY_ENUM:
        return strcmp(a -> named.name, b -> named.name) == 0;
    case TY_GENERIC:
        return strcmp(a -> generic.param, b -> generic.param) == 0;
    default:
        return true;
    }
}

const char * type_to_llvm(Type * t) {
    if (!t) return "void";
    switch (t -> kind) {
    case TY_VOID:
        return "void";
    case TY_BOOL:
        return "i1";
    case TY_I8:
        return "i8";
    case TY_I16:
        return "i16";
    case TY_I32:
        return "i32";
    case TY_I64:
        return "i64";
    case TY_U8:
        return "i8";
    case TY_U16:
        return "i16";
    case TY_U32:
        return "i32";
    case TY_U64:
        return "i64";
    case TY_F32:
        return "float";
    case TY_F64:
        return "double";
    case TY_STRING:
        return "ptr";
    case TY_CHAR:
        return "i8";
    case TY_INT_GENERIC:
        return "i64";
    case TY_FLOAT_GENERIC:
        return "double";
    case TY_GENERIC:
        return "ptr";
    case TY_PTR: {
        return "ptr";
    }
    case TY_ARRAY: {
        static char buf[256];
        snprintf(buf, sizeof(buf), "[%lld x %s]",
            (long long) t -> array.count, type_to_llvm(t -> array.elem));
        return arena_strdup(g_arena, buf);
    }
    case TY_STRUCT:
        return arena_sprintf(g_arena, "%%struct.%s", t -> named.name);
    case TY_ENUM:
        return arena_sprintf(g_arena, "%%enum.%s", t -> named.name);
    default:
        return "ptr";
    }
}

const char * type_to_llvm_elem(Type * t) {
    if (t -> kind == TY_ARRAY) return type_to_llvm(t -> array.elem);
    return type_to_llvm(t);
}

/* ================================================================
   PARSER HELPERS
   ================================================================ */

Parser * parser_new(Arena * a, Lexer * l) {
    Parser * p = arena_alloc(a, sizeof(Parser));
    p -> lexer = l;
    p -> arena = a;
    p -> had_error = false;
    p -> panic = false;
    p -> cur = lexer_next(l);
    return p;
}

static Token advance_tok(Parser * p) {
    p -> prev = p -> cur;
    p -> cur = lexer_next(p -> lexer);
    return p -> prev;
}

static bool check(Parser * p, TokenType t) {
    return p -> cur.type == t;
}
static bool is_type_start(TokenType t);

static bool match_tok(Parser * p, TokenType t) {
    if (check(p, t)) {
        advance_tok(p);
        return true;
    }
    return false;
}

static Token expect(Parser * p, TokenType t,
    const char * msg) {
    if (p -> cur.type != t) {
        if (!p -> panic) {
            lc_error_tok(p -> cur.line, p -> cur.col, p -> cur.length,
                "Expected %s, got %s",
                msg, token_type_name(p -> cur.type));
            p -> had_error = true;
            p -> panic = true;
        }
        return p -> cur;
    }
    p -> panic = false;
    return advance_tok(p);
}

#define DA_PUSH(arena, arr, cnt, cap, val) do {
    \
    if ((cnt) >= (cap)) {
        \
        int newcap = (cap) == 0 ? 8 : (cap) * 2;\
        void ** tmp = arena_alloc((arena), sizeof(void * ) * (size_t) newcap);\
        if (arr) memcpy(tmp, (arr), sizeof(void * ) * (size_t)(cnt));\
        (arr) = (void * ) tmp;
        (cap) = newcap;\
    }\
    ((void ** )(arr))[(cnt) ++] = (void * )(uintptr_t)(val);\
} while (0)

static void ** vec_push(Arena * a, void ** arr, int * cnt, int * cap, void * val) {
    if ( * cnt >= * cap) {
        int nc = * cap == 0 ? 8 : * cap * 2;
        void ** tmp = arena_alloc(a, sizeof(void * ) * (size_t) nc);
        if (arr) memcpy(tmp, arr, sizeof(void * ) * (size_t) * cnt);
        arr = tmp;* cap = nc;
    }
    arr[( * cnt) ++] = val;
    return arr;
}

/* ================================================================
   TYPE PARSING
   ================================================================ */

static void synchronize(Parser * p) {
    p -> panic = false;
    while (!check(p, TOK_EOF)) {
        if (p -> prev.type == TOK_SEMICOLON) return;
        switch (p -> cur.type) {
        case TOK_SEMICOLON:
            advance_tok(p);
            return;
        case TOK_STRUCT:
        case TOK_ENUM:
        case TOK_EXTERN:
        case TOK_UNSAFE:
        case TOK_IMPORT:
        case TOK_IN:
        case TOK_PACKED:
        case TOK_NOMAIN_DIR:
            return;
        case TOK_IDENT: {
            Token pk = lexer_peek(p -> lexer);
            if (pk.type == TOK_LPAREN) return;
            break;
        }
        default:
            break;
        }
        advance_tok(p);
    }
}

static Type * parse_type(Parser * p);

static Type * parse_type(Parser * p) {
    Arena * a = p -> arena;
    Type * t = NULL;

    bool nomd = match_tok(p, TOK_NOMD);
    (void) nomd;

    switch (p -> cur.type) {
    case TOK_VOID_KW:
        advance_tok(p);
        t = make_type(a, TY_VOID);
        break;
    case TOK_BOOL_KW:
        advance_tok(p);
        t = make_type(a, TY_BOOL);
        break;
    case TOK_I8:
        advance_tok(p);
        t = make_type(a, TY_I8);
        break;
    case TOK_I16:
        advance_tok(p);
        t = make_type(a, TY_I16);
        break;
    case TOK_I32:
        advance_tok(p);
        t = make_type(a, TY_I32);
        break;
    case TOK_I64:
        advance_tok(p);
        t = make_type(a, TY_I64);
        break;
    case TOK_U8:
        advance_tok(p);
        t = make_type(a, TY_U8);
        break;
    case TOK_U16:
        advance_tok(p);
        t = make_type(a, TY_U16);
        break;
    case TOK_U32:
        advance_tok(p);
        t = make_type(a, TY_U32);
        break;
    case TOK_U64:
        advance_tok(p);
        t = make_type(a, TY_U64);
        break;
    case TOK_F32:
        advance_tok(p);
        t = make_type(a, TY_F32);
        break;
    case TOK_F64:
        advance_tok(p);
        t = make_type(a, TY_F64);
        break;
    case TOK_STRING_KW:
        advance_tok(p);
        t = make_type(a, TY_STRING);
        break;
    case TOK_CHAR_KW:
        advance_tok(p);
        t = make_type(a, TY_CHAR);
        break;
    case TOK_INT_KW:
        advance_tok(p);
        t = make_type(a, TY_INT_GENERIC);
        break;
    case TOK_FLOAT_KW:
        advance_tok(p);
        t = make_type(a, TY_FLOAT_GENERIC);
        break;
    case TOK_HASH:
        advance_tok(p);
        t = make_type(a, TY_CONTEXT);
        break;
    case TOK_STAR:
        advance_tok(p);
        t = make_type(a, TY_PTR);
        t -> ptr.pointee = parse_type(p);
        break;
    case TOK_IDENT: {

        const char * name = arena_strdup(a, p -> cur.sval);
        advance_tok(p);
        t = make_type(a, TY_STRUCT);
        t -> named.name = name;

        if (check(p, TOK_LANGLE)) {
            advance_tok(p);

            parse_type(p);

            while (match_tok(p, TOK_COMMA)) parse_type(p);
            expect(p, TOK_RANGLE, ">");
        }
        break;
    }

    default:
        lc_error(p -> cur.line, p -> cur.col,
            "expected type, got '%.*s'", p -> cur.length, p -> cur.start);
        p -> had_error = true;
        t = make_type(a, TY_VOID);
        break;
    }

    while (check(p, TOK_LBRACKET)) {
        advance_tok(p);
        Type * arr = make_type(a, TY_ARRAY);
        arr -> array.elem = t;
        if (!check(p, TOK_RBRACKET)) {
            Token lit = expect(p, TOK_INT_LIT, "array size");
            arr -> array.count = lit.ival;
        } else {
            arr -> array.count = -1;
        }
        expect(p, TOK_RBRACKET, "]");
        t = arr;
    }

    return t;
}

static Type * parse_angle_type(Parser * p) {
    expect(p, TOK_LANGLE, "<");
    Type * t = parse_type(p);
    expect(p, TOK_RANGLE, ">");
    return t;
}

/* ================================================================
   EXPRESSION PARSING (Pratt / recursive descent)
   ================================================================ */

static ASTNode * parse_expr(Parser * p);
static ASTNode * parse_block(Parser * p);

typedef enum {
    PREC_NONE = 0,
        PREC_ASSIGN,
        PREC_TERNARY,
        PREC_OR,
        PREC_AND,
        PREC_BIT_OR,
        PREC_BIT_XOR,
        PREC_BIT_AND,
        PREC_EQ,
        PREC_CMP,
        PREC_SHIFT,
        PREC_ADD,
        PREC_MUL,
        PREC_UNARY,
        PREC_CALL,
}
Prec;

static Prec token_prec(TokenType t) {
    switch (t) {
    case TOK_EQ:
    case TOK_PLUSEQ:
    case TOK_MINUSEQ:
    case TOK_STAREQ:
    case TOK_SLASHEQ:
    case TOK_PERCENTEQ:
    case TOK_AMPEQ:
    case TOK_PIPEEQ:
    case TOK_CARETEQ:
    case TOK_LSHIFTEQ:
    case TOK_RSHIFTEQ:
        return PREC_ASSIGN;
    case TOK_QUESTION:
        return PREC_TERNARY;
    case TOK_OROR:
        return PREC_OR;
    case TOK_ANDAND:
        return PREC_AND;
    case TOK_PIPE:
        return PREC_BIT_OR;
    case TOK_CARET:
        return PREC_BIT_XOR;
    case TOK_AMP:
        return PREC_BIT_AND;
    case TOK_EQEQ:
    case TOK_NEQ:
        return PREC_EQ;
    case TOK_LANGLE:
    case TOK_RANGLE:
    case TOK_LTE:
    case TOK_GTE:
        return PREC_CMP;
    case TOK_LSHIFT:
    case TOK_RSHIFT:
        return PREC_SHIFT;
    case TOK_PLUS:
    case TOK_MINUS:
        return PREC_ADD;
    case TOK_STAR:
    case TOK_SLASH:
    case TOK_PERCENT:
        return PREC_MUL;
    default:
        return PREC_NONE;
    }
}

static bool is_assign_op(TokenType t) {
    switch (t) {
    case TOK_EQ:
    case TOK_PLUSEQ:
    case TOK_MINUSEQ:
    case TOK_STAREQ:
    case TOK_SLASHEQ:
    case TOK_PERCENTEQ:
    case TOK_AMPEQ:
    case TOK_PIPEEQ:
    case TOK_CARETEQ:
    case TOK_LSHIFTEQ:
    case TOK_RSHIFTEQ:
        return true;
    default:
        return false;
    }
}

static ASTNode * parse_primary(Parser * p) {
    Arena * a = p -> arena;
    int ln = p -> cur.line;

    if (check(p, TOK_LPAREN)) {

        Token pk = lexer_peek(p -> lexer);
        bool is_type_kw = (pk.type >= TOK_I8 && pk.type <= TOK_FLOAT_KW) ||
            pk.type == TOK_VOID_KW || pk.type == TOK_BOOL_KW ||
            pk.type == TOK_STRING_KW || pk.type == TOK_CHAR_KW;
        if (is_type_kw) {
            advance_tok(p);
            Type * target = parse_type(p);
            expect(p, TOK_RPAREN, ")");
            ASTNode * operand = parse_expr(p);
            ASTNode * n = make_node(a, ND_CAST, ln);
            n -> cast.target = target;
            n -> cast.operand = operand;
            return n;
        }
    }

    if (check(p, TOK_STRING_LIT)) {
        Token tok = advance_tok(p);

        bool is_fmt = false;
        for (const char * s = tok.sval;* s; s++) {
            if ( * s == '{') {
                is_fmt = true;
                break;
            }
        }
        if (is_fmt) {
            ASTNode * n = make_node(a, ND_FORMAT_STR, ln);
            n -> fmt_str.raw = tok.sval;
            return n;
        }
        ASTNode * n = make_node(a, ND_STRING_LIT, ln);
        n -> str_lit.val = tok.sval;
        return n;
    }

    switch (p -> cur.type) {
    case TOK_INT_LIT: {
        Token tok = advance_tok(p);
        ASTNode * n = make_node(a, ND_INT_LIT, ln);
        n -> int_lit.val = tok.ival;
        return n;
    }
    case TOK_FLOAT_LIT: {
        Token tok = advance_tok(p);
        ASTNode * n = make_node(a, ND_FLOAT_LIT, ln);
        n -> float_lit.val = tok.fval;
        return n;
    }
    case TOK_CHAR_LIT: {
        Token tok = advance_tok(p);
        ASTNode * n = make_node(a, ND_CHAR_LIT, ln);
        n -> char_lit.val = (char) tok.ival;
        return n;
    }
    case TOK_TRUE: {
        advance_tok(p);
        ASTNode * n = make_node(a, ND_BOOL_LIT, ln);
        n -> bool_lit.val = true;
        return n;
    }
    case TOK_FALSE: {
        advance_tok(p);
        ASTNode * n = make_node(a, ND_BOOL_LIT, ln);
        n -> bool_lit.val = false;
        return n;
    }
    case TOK_HASH: {
        advance_tok(p);
        return make_node(a, ND_HASH, ln);
    }

    case TOK_LBRACKET: {
        advance_tok(p);
        ASTNode ** elems = NULL;
        int cnt = 0, cap = 0;
        while (!check(p, TOK_RBRACKET) && !check(p, TOK_EOF)) {
            ASTNode ** tmp = (ASTNode ** ) vec_push(a, (void ** ) elems, & cnt, & cap, parse_expr(p));
            elems = tmp;
            if (!match_tok(p, TOK_COMMA)) break;
        }
        expect(p, TOK_RBRACKET, "]");
        ASTNode * n = make_node(a, ND_ARRAY_LIT, ln);
        n -> array_lit.elems = elems;
        n -> array_lit.count = cnt;
        return n;
    }

    case TOK_LPAREN: {
        advance_tok(p);
        ASTNode * e = parse_expr(p);
        expect(p, TOK_RPAREN, ")");
        return e;
    }

    case TOK_IDENT: {
        const char * name = arena_strdup(a, p -> cur.sval);
        advance_tok(p);

        if (check(p, TOK_ARROW)) {
            advance_tok(p);
            const char * variant;
            if (check(p, TOK_IDENT)) {
                variant = arena_strdup(a, p -> cur.sval);
                advance_tok(p);
            } else {
                lc_error(ln, 0, "expected variant name after '->'");
                variant = "?";
            }
            ASTNode * n = make_node(a, ND_ENUM_FIELD, ln);
            n -> enum_field.enum_name = name;
            n -> enum_field.variant = variant;
            n -> enum_field.args = NULL;
            n -> enum_field.arg_count = 0;

            if (check(p, TOK_LPAREN)) {
                advance_tok(p);
                ASTNode ** args = NULL;
                int ac = 0, acap = 0;
                while (!check(p, TOK_RPAREN) && !check(p, TOK_EOF)) {
                    args = (ASTNode ** ) vec_push(a, (void ** ) args, & ac, & acap, parse_expr(p));
                    if (!match_tok(p, TOK_COMMA)) break;
                }
                expect(p, TOK_RPAREN, ")");
                n -> enum_field.args = args;
                n -> enum_field.arg_count = ac;
            }
            return n;
        }

        if (check(p, TOK_LBRACE)) {

            advance_tok(p);
            ASTNode * n = make_node(a, ND_STRUCT_INIT, ln);
            n -> struct_init.struct_name = name;
            int fc = 0, fcap = 8;
            const char ** fnames = (const char ** ) arena_alloc(a, sizeof(void * ) * 8);
            ASTNode ** fvals = (ASTNode ** ) arena_alloc(a, sizeof(void * ) * 8);
            while (!check(p, TOK_RBRACE) && !check(p, TOK_EOF)) {
                const char * fname = p -> cur.sval ? arena_strdup(a, p -> cur.sval) : "?";
                expect(p, TOK_IDENT, "field name");
                expect(p, TOK_LPAREN, "(");
                ASTNode * fval = parse_expr(p);
                expect(p, TOK_RPAREN, ")");
                expect(p, TOK_SEMICOLON, ";");
                if (fc >= fcap) {
                    int nc = fcap * 2;
                    const char ** nn = (const char ** ) arena_alloc(a, sizeof(void * ) * nc);
                    ASTNode ** nv = (ASTNode ** ) arena_alloc(a, sizeof(void * ) * nc);
                    memcpy(nn, fnames, sizeof(void * ) * (size_t) fc);
                    memcpy(nv, fvals, sizeof(void * ) * (size_t) fc);
                    fnames = nn;
                    fvals = nv;
                    fcap = nc;
                }
                fnames[fc] = fname;
                fvals[fc] = fval;
                fc++;
            }
            expect(p, TOK_RBRACE, "}");
            n -> struct_init.field_names = fnames;
            n -> struct_init.field_vals = fvals;
            n -> struct_init.field_count = fc;
            return n;
        }

        ASTNode * n = make_node(a, ND_IDENT, ln);
        n -> ident.name = name;
        return n;
    }

    default:
        lc_error(p -> cur.line, p -> cur.col,
            "unexpected token '%.*s' in expression",
            p -> cur.length, p -> cur.start);
        p -> had_error = true;
        advance_tok(p);
        return make_node(a, ND_INT_LIT, ln);
    }
}

static ASTNode * parse_postfix(Parser * p, ASTNode * base) {
    Arena * a = p -> arena;
    for (;;) {
        int ln = p -> cur.line;

        if (check(p, TOK_LPAREN)) {
            advance_tok(p);
            ASTNode ** args = NULL;
            int ac = 0, acap = 0;
            while (!check(p, TOK_RPAREN) && !check(p, TOK_EOF)) {
                args = (ASTNode ** ) vec_push(a, (void ** ) args, & ac, & acap, parse_expr(p));
                if (!match_tok(p, TOK_COMMA)) break;
            }

            match_tok(p, TOK_COMMA);
            expect(p, TOK_RPAREN, ")");
            ASTNode * n = make_node(a, ND_CALL, ln);
            n -> call.callee = base;
            n -> call.args = args;
            n -> call.arg_count = ac;
            base = n;
            continue;
        }

        if (check(p, TOK_LBRACKET)) {
            advance_tok(p);
            ASTNode * idx = parse_expr(p);
            expect(p, TOK_RBRACKET, "]");
            ASTNode * n = make_node(a, ND_INDEX, ln);
            n -> idx.array = base;
            n -> idx.index = idx;
            base = n;
            continue;
        }

        if (check(p, TOK_DOT)) {
            advance_tok(p);
            const char * fname = p -> cur.sval ? arena_strdup(a, p -> cur.sval) : "?";
            expect(p, TOK_IDENT, "field name");
            ASTNode * n = make_node(a, ND_FIELD, ln);
            n -> field.object = base;
            n -> field.field = fname;
            base = n;
            continue;
        }

        break;
    }
    return base;
}

static ASTNode * parse_unary(Parser * p) {
    Arena * a = p -> arena;
    int ln = p -> cur.line;
    switch (p -> cur.type) {
    case TOK_MINUS:
    case TOK_BANG:
    case TOK_TILDE:
    case TOK_STAR:
    case TOK_AMP: {
        TokenType op = p -> cur.type;
        advance_tok(p);
        ASTNode * operand = parse_unary(p);
        ASTNode * n = make_node(a, ND_UNARY, ln);
        n -> unary.op = op;
        n -> unary.operand = operand;
        return n;
    }
    default: {
        ASTNode * base = parse_primary(p);
        return parse_postfix(p, base);
    }
    }
}

static ASTNode * parse_binary(Parser * p, int min_prec) {
    ASTNode * lhs = parse_unary(p);

    for (;;) {
        int prec = (int) token_prec(p -> cur.type);
        if (prec <= min_prec) break;

        if (p -> cur.type == TOK_QUESTION) {
            advance_tok(p);
            ASTNode * then_val = parse_expr(p);
            expect(p, TOK_COLON, ":");
            ASTNode * else_val = parse_expr(p);
            ASTNode * n = make_node(p -> arena, ND_TERNARY, lhs -> line);
            n -> ternary.cond = lhs;
            n -> ternary.then_val = then_val;
            n -> ternary.else_val = else_val;
            return n;
        }

        if (is_assign_op(p -> cur.type)) {
            TokenType op = p -> cur.type;
            advance_tok(p);
            ASTNode * rhs = parse_binary(p, PREC_ASSIGN - 1);
            ASTNode * n = make_node(p -> arena, ND_ASSIGN, lhs -> line);
            n -> assign.lhs = lhs;
            n -> assign.rhs = rhs;
            n -> assign.op = op;
            return n;
        }

        TokenType op = p -> cur.type;
        advance_tok(p);
        ASTNode * rhs = parse_binary(p, prec);

        ASTNode * n = make_node(p -> arena, ND_BINARY, lhs -> line);
        n -> binary.op = op;
        n -> binary.lhs = lhs;
        n -> binary.rhs = rhs;
        lhs = n;
    }
    return lhs;
}

static ASTNode * parse_expr(Parser * p) {
    return parse_binary(p, PREC_NONE);
}

/* ================================================================
   STATEMENT PARSING
   ================================================================ */

static ASTNode * parse_stmt(Parser * p);
static bool stmt_terminator(Parser * p) {
    if (match_tok(p, TOK_SEMICOLON)) return true;
    if (check(p, TOK_RBRACE) || check(p, TOK_EOF)) return true;
    lc_error(p -> cur.line, p -> cur.col, "expected ';' or end of block");
    return false;
}

static ASTNode * parse_block(Parser * p) {
    Arena * a = p -> arena;
    int ln = p -> cur.line;
    expect(p, TOK_LBRACE, "{");
    ASTNode * n = make_node(a, ND_BLOCK, ln);
    ASTNode ** stmts = NULL;
    int cnt = 0, cap = 0;

    while (!check(p, TOK_RBRACE) && !check(p, TOK_EOF)) {
        ASTNode * s = parse_stmt(p);
        if (s) stmts = (ASTNode ** ) vec_push(a, (void ** ) stmts, & cnt, & cap, s);
    }
    expect(p, TOK_RBRACE, "}");
    n -> block.stmts = stmts;
    n -> block.count = cnt;
    return n;
}

static ASTNode * parse_crumble_stmt(Parser * p) {
    Arena * a = p -> arena;
    int ln = p -> cur.line;
    ASTNode * n = make_node(a, ND_CRUMBLE, ln);
    TokenType kind = p -> cur.type;
    advance_tok(p);

    n -> crumble.read_limit = -1;
    n -> crumble.write_limit = -1;
    n -> crumble.is_dropall = (kind == TOK_DROPALL);
    n -> crumble.is_readonly = (kind == TOK_READONLY);
    n -> crumble.is_writeonly = (kind == TOK_WRITEONLY);

    if (kind == TOK_DROPALL) {
        n -> crumble.read_limit = 0;
        n -> crumble.write_limit = 0;
    }
    if (kind == TOK_READONLY) {
        n -> crumble.write_limit = 0;
    }
    if (kind == TOK_WRITEONLY) {
        n -> crumble.read_limit = 0;
    }

    expect(p, TOK_LPAREN, "(");
    const char ** vars = NULL;
    int vc = 0, vcap = 0;
    while (!check(p, TOK_RPAREN) && !check(p, TOK_EOF)) {
        const char * vname = p -> cur.sval ? arena_strdup(a, p -> cur.sval) : "?";
        expect(p, TOK_IDENT, "variable name");
        vars = (const char ** ) vec_push(a, (void ** ) vars, & vc, & vcap, (void * ) vname);
        if (!match_tok(p, TOK_COMMA)) break;
    }
    expect(p, TOK_RPAREN, ")");

    if (kind == TOK_CRUMBLE) {
        while (check(p, TOK_BANG)) {
            advance_tok(p);
            if (check(p, TOK_IDENT) && strcmp(p -> cur.sval, "r") == 0) {
                advance_tok(p);
                expect(p, TOK_EQ, "=");
                Token lit = expect(p, TOK_INT_LIT, "integer");
                n -> crumble.read_limit = (int) lit.ival;
            } else if (check(p, TOK_IDENT) && strcmp(p -> cur.sval, "w") == 0) {
                advance_tok(p);
                expect(p, TOK_EQ, "=");
                Token lit = expect(p, TOK_INT_LIT, "integer");
                n -> crumble.write_limit = (int) lit.ival;
            } else {
                lc_error(ln, 0, "expected !r= or !w= in crumble");
                advance_tok(p);
            }
        }
    }

    n -> crumble.vars = vars;
    n -> crumble.var_count = vc;
    stmt_terminator(p);
    return n;
}

static ASTNode * parse_match(Parser * p) {
    Arena * a = p -> arena;
    int ln = p -> cur.line;
    advance_tok(p);
    expect(p, TOK_LPAREN, "(");
    ASTNode * subject = parse_expr(p);
    expect(p, TOK_RPAREN, ")");
    expect(p, TOK_LBRACE, "{");

    ASTNode ** arms = NULL;
    int ac = 0, acap = 0;

    while (!check(p, TOK_RBRACE) && !check(p, TOK_EOF)) {
        int aln = p -> cur.line;
        const char * variant = p -> cur.sval ? arena_strdup(a, p -> cur.sval) : "?";
        expect(p, TOK_IDENT, "variant name");

        ASTNode * arm = make_node(a, ND_MATCH_ARM, aln);
        arm -> match_arm.variant = variant;
        arm -> match_arm.bind_name = NULL;

        if (check(p, TOK_LPAREN)) {
            advance_tok(p);
            if (check(p, TOK_IDENT)) {
                arm -> match_arm.bind_name = arena_strdup(a, p -> cur.sval);
                advance_tok(p);
            }
            expect(p, TOK_RPAREN, ")");
        }

        arm -> match_arm.body = parse_block(p);
        arms = (ASTNode ** ) vec_push(a, (void ** ) arms, & ac, & acap, arm);
    }
    expect(p, TOK_RBRACE, "}");

    ASTNode * n = make_node(a, ND_MATCH, ln);
    n -> match.subject = subject;
    n -> match.arms = arms;
    n -> match.arm_count = ac;
    return n;
}

static ASTNode * parse_typeswitch(Parser * p) {
    Arena * a = p -> arena;
    int ln = p -> cur.line;
    advance_tok(p);
    expect(p, TOK_LPAREN, "(");

    const char * subj;
    if (check(p, TOK_HASH)) {
        advance_tok(p);
        subj = "#";
    } else {
        if (!check(p, TOK_IDENT)) {
            lc_error(p -> cur.line, p -> cur.col,
                "expected candidate name or '#' in typeswitch");
            p -> had_error = true;
            subj = "?";
        } else {
            subj = p -> cur.sval ? arena_strdup(a, p -> cur.sval) : "?";
            advance_tok(p);
        }
    }
    expect(p, TOK_RPAREN, ")");
    expect(p, TOK_LBRACE, "{");

    ASTNode ** cases = NULL;
    int cc = 0, ccap = 0;
    ASTNode * fallback = NULL;

    while (!check(p, TOK_RBRACE) && !check(p, TOK_EOF)) {
        if (check(p, TOK_FALLBACK)) {
            advance_tok(p);
            fallback = parse_block(p);
        } else {
            expect(p, TOK_TYPECASE, "typecase");
            expect(p, TOK_LPAREN, "(");
            Type * mt = parse_type(p);
            expect(p, TOK_RPAREN, ")");
            ASTNode * body = parse_block(p);
            ASTNode * tc = make_node(a, ND_TYPECASE, ln);
            tc -> typecase.match_type = mt;
            tc -> typecase.body = body;
            cases = (ASTNode ** ) vec_push(a, (void ** ) cases, & cc, & ccap, tc);
        }
    }
    expect(p, TOK_RBRACE, "}");

    ASTNode * n = make_node(a, ND_TYPESWITCH, ln);
    n -> typeswitch.subject_name = subj;
    n -> typeswitch.cases = cases;
    n -> typeswitch.case_count = cc;
    n -> typeswitch.fallback = fallback;
    return n;
}

static ASTNode * parse_stmt(Parser * p) {
    Arena * a = p -> arena;
    int ln = p -> cur.line;

    bool is_nomd = check(p, TOK_NOMD);
    bool starts_type = is_nomd ||
        (p -> cur.type >= TOK_I8 && p -> cur.type <= TOK_FLOAT_KW) ||
        p -> cur.type == TOK_VOID_KW || p -> cur.type == TOK_BOOL_KW ||
        p -> cur.type == TOK_STRING_KW || p -> cur.type == TOK_CHAR_KW;

    if (!starts_type && check(p, TOK_IDENT)) {

        Token next = lexer_peek(p -> lexer);
        if (next.type == TOK_IDENT || next.type == TOK_LANGLE) {
            starts_type = true;
        }
    }

    if (starts_type && !check(p, TOK_IDENT) == false) {

    }

    if (check(p, TOK_RET)) {
        advance_tok(p);
        ASTNode * n = make_node(a, ND_RETURN, ln);
        if (!check(p, TOK_SEMICOLON) && !check(p, TOK_RBRACE) && !check(p, TOK_EOF)) {
            n -> ret.val = parse_expr(p);
        }
        stmt_terminator(p);
        return n;
    }

    if (check(p, TOK_IF)) {
        advance_tok(p);
        expect(p, TOK_LPAREN, "(");
        ASTNode * cond = parse_expr(p);
        expect(p, TOK_RPAREN, ")");
        ASTNode * then_b = parse_block(p);
        ASTNode * else_b = NULL;
        if (check(p, TOK_ELSE)) {
            advance_tok(p);
            if (check(p, TOK_IF)) else_b = parse_stmt(p);
            else else_b = parse_block(p);
        }
        ASTNode * n = make_node(a, ND_IF, ln);
        n -> ifstmt.cond = cond;
        n -> ifstmt.then_body = then_b;
        n -> ifstmt.else_body = else_b;
        return n;
    }

    if (check(p, TOK_LOOP)) {
        advance_tok(p);
        ASTNode * n = make_node(a, ND_LOOP, ln);
        n -> loop.counter = n -> loop.cond = n -> loop.step = NULL;

        if (check(p, TOK_LPAREN)) {
            advance_tok(p);

            if (!check(p, TOK_COMMA) && !check(p, TOK_RPAREN)) {

                bool is_ty = is_type_start(p -> cur.type) ||
                    (p -> cur.type == TOK_IDENT && is_type_start(lexer_peek(p -> lexer).type));
                if (is_ty || (p -> cur.type == TOK_I64 || p -> cur.type == TOK_I32 ||
                        p -> cur.type == TOK_I8 || p -> cur.type == TOK_I16 ||
                        p -> cur.type == TOK_U64 || p -> cur.type == TOK_U32 ||
                        p -> cur.type == TOK_U8 || p -> cur.type == TOK_U16 ||
                        p -> cur.type == TOK_F32 || p -> cur.type == TOK_F64)) {
                    int cln = p -> cur.line;
                    Type * cty = parse_type(p);
                    const char * cname = p -> cur.sval ? arena_strdup(a, p -> cur.sval) : "?";
                    expect(p, TOK_IDENT, "counter name");
                    ASTNode * cv = make_node(a, ND_VAR_DECL, cln);
                    cv ->
                        var.name = cname;
                    cv ->
                        var.type = cty;
                    cv ->
                        var.nomd = false;
                    if (match_tok(p, TOK_EQ))
                        cv ->
                        var.init = parse_expr(p);
                    n -> loop.counter = cv;
                } else {

                    ASTNode * ce = parse_expr(p);
                    ASTNode * cs = make_node(a, ND_EXPR_STMT, ce -> line);
                    cs -> expr_stmt.expr = ce;
                    n -> loop.counter = cs;
                }
            }
            if (match_tok(p, TOK_COMMA)) {
                if (!check(p, TOK_COMMA) && !check(p, TOK_RPAREN))
                    n -> loop.cond = parse_expr(p);
            }
            if (match_tok(p, TOK_COMMA)) {
                if (!check(p, TOK_RPAREN))
                    n -> loop.step = parse_expr(p);
            }
            expect(p, TOK_RPAREN, ")");
        }
        n -> loop.body = parse_block(p);
        return n;
    }

    if (check(p, TOK_MATCH)) return parse_match(p);

    if (check(p, TOK_TYPESWITCH)) return parse_typeswitch(p);

    if (check(p, TOK_BR)) {
        advance_tok(p);
        stmt_terminator(p);
        return make_node(a, ND_BREAK, ln);
    }
    if (check(p, TOK_CONT)) {
        advance_tok(p);
        stmt_terminator(p);
        return make_node(a, ND_CONTINUE, ln);
    }

    if (check(p, TOK_CRUMBLE) || check(p, TOK_DROPALL) ||
        check(p, TOK_READONLY) || check(p, TOK_WRITEONLY)) {
        return parse_crumble_stmt(p);
    }

    if (starts_type) {
        bool nomd = match_tok(p, TOK_NOMD);
        Type * ty = parse_type(p);

        const char * vname = p -> cur.sval ? arena_strdup(a, p -> cur.sval) : "?";

        if (check(p, TOK_IDENT)) {
            advance_tok(p);
            ASTNode * n = make_node(a, ND_VAR_DECL, ln);
            n ->
                var.name = vname;
            n ->
                var.type = ty;
            n ->
                var.nomd = nomd;
            if (match_tok(p, TOK_EQ)) {
                n ->
                    var.init = parse_expr(p);
            }
            stmt_terminator(p);
            return n;
        }

        lc_error(ln, 0, "expected variable name after type");
    }

    {
        ASTNode * e = parse_expr(p);
        stmt_terminator(p);

        if (e -> kind == ND_ASSIGN) return e;
        ASTNode * n = make_node(a, ND_EXPR_STMT, ln);
        n -> expr_stmt.expr = e;
        return n;
    }
}

/* ================================================================
   TOP-LEVEL DECLARATIONS
   ================================================================ */

static bool is_type_start(TokenType t) {
    switch (t) {
    case TOK_VOID_KW:
    case TOK_BOOL_KW:
    case TOK_I8:
    case TOK_I16:
    case TOK_I32:
    case TOK_I64:
    case TOK_U8:
    case TOK_U16:
    case TOK_U32:
    case TOK_U64:
    case TOK_F32:
    case TOK_F64:
    case TOK_STRING_KW:
    case TOK_CHAR_KW:
    case TOK_INT_KW:
    case TOK_FLOAT_KW:
    case TOK_HASH:
    case TOK_STAR:
    case TOK_LANGLE:
        return true;
    default:
        return false;
    }
}

static ASTNode * parse_func_or_global(Parser * p, bool is_extern, bool is_unsafe, bool is_packed) {
    Arena * a = p -> arena;
    int ln = p -> cur.line;

    if (is_packed || check(p, TOK_STRUCT)) {
        if (!is_packed) advance_tok(p);

        const char * sname = p -> cur.sval ? arena_strdup(a, p -> cur.sval) : "?";
        expect(p, TOK_IDENT, "struct name");
        expect(p, TOK_LBRACE, "{");

        const char ** fnames = NULL;
        Type ** ftypes = NULL;
        bool * fnomds = NULL;
        int fc = 0, fcap = 8;
        fnames = (const char ** ) arena_alloc(a, sizeof(void * ) * 8);
        ftypes = (Type ** ) arena_alloc(a, sizeof(void * ) * 8);
        fnomds = (bool * ) arena_alloc(a, sizeof(void * ) * 8);

        while (!check(p, TOK_RBRACE) && !check(p, TOK_EOF)) {
            bool fn = match_tok(p, TOK_NOMD);
            Type * ft = parse_type(p);
            const char * fname = p -> cur.sval ? arena_strdup(a, p -> cur.sval) : "?";
            expect(p, TOK_IDENT, "field name");
            expect(p, TOK_SEMICOLON, ";");
            if (fc >= fcap) {
                int nc = fcap * 2;
                const char ** nn = (const char ** ) arena_alloc(a, sizeof(void * ) * nc);
                Type ** nt = (Type ** ) arena_alloc(a, sizeof(void * ) * nc);
                bool * nb = (bool * ) arena_alloc(a, sizeof(void * ) * nc);
                memcpy(nn, fnames, sizeof(void * ) * (size_t) fc);
                memcpy(nt, ftypes, sizeof(void * ) * (size_t) fc);
                memcpy(nb, fnomds, sizeof(void * ) * (size_t) fc);
                fnames = nn;
                ftypes = nt;
                fnomds = nb;
                fcap = nc;
            }
            fnames[fc] = fname;
            ftypes[fc] = ft;
            fnomds[fc] = fn;
            fc++;
        }
        expect(p, TOK_RBRACE, "}");

        ASTNode * n = make_node(a, ND_STRUCT_DECL, ln);
        n -> strct.name = sname;
        n -> strct.field_names = fnames;
        n -> strct.field_types = ftypes;
        n -> strct.field_nomd = fnomds;
        n -> strct.field_count = fc;
        n -> strct.packed = is_packed;
        return n;
    }

    if (check(p, TOK_ENUM)) {
        advance_tok(p);
        const char * ename = p -> cur.sval ? arena_strdup(a, p -> cur.sval) : "?";
        expect(p, TOK_IDENT, "enum name");
        const char * gparam = NULL;
        if (check(p, TOK_LANGLE)) {
            advance_tok(p);
            gparam = p -> cur.sval ? arena_strdup(a, p -> cur.sval) : "T";
            expect(p, TOK_IDENT, "generic param");

            while (match_tok(p, TOK_COMMA)) {

                advance_tok(p);
            }
            expect(p, TOK_RANGLE, ">");
        }
        expect(p, TOK_LBRACE, "{");

        const char ** vnames = NULL;
        Type ** vtypes = NULL;
        int vc = 0, vcap = 8;
        vnames = (const char ** ) arena_alloc(a, sizeof(void * ) * 8);
        vtypes = (Type ** ) arena_alloc(a, sizeof(void * ) * 8);

        while (!check(p, TOK_RBRACE) && !check(p, TOK_EOF)) {
            const char * vname = p -> cur.sval ? arena_strdup(a, p -> cur.sval) : "?";
            expect(p, TOK_IDENT, "variant name");
            Type * vtype = NULL;
            if (check(p, TOK_LPAREN)) {
                advance_tok(p);
                vtype = parse_type(p);
                expect(p, TOK_RPAREN, ")");
            }
            expect(p, TOK_SEMICOLON, ";");
            if (vc >= vcap) {
                int nc = vcap * 2;
                const char ** nn = (const char ** ) arena_alloc(a, sizeof(void * ) * nc);
                Type ** nt = (Type ** ) arena_alloc(a, sizeof(void * ) * nc);
                memcpy(nn, vnames, sizeof(void * ) * (size_t) vc);
                memcpy(nt, vtypes, sizeof(void * ) * (size_t) vc);
                vnames = nn;
                vtypes = nt;
                vcap = nc;
            }
            vnames[vc] = vname;
            vtypes[vc] = vtype;
            vc++;
        }
        expect(p, TOK_RBRACE, "}");

        ASTNode * n = make_node(a, ND_ENUM_DECL, ln);
        n -> enm.name = ename;
        n -> enm.generic_param = gparam;
        n -> enm.variant_names = vnames;
        n -> enm.variant_types = vtypes;
        n -> enm.variant_count = vc;
        return n;
    }

    if (check(p, TOK_IDENT)) {
        Token pk = lexer_peek(p -> lexer);
        if (pk.type == TOK_LPAREN) {

            const char * fname = arena_strdup(a, p -> cur.sval);
            advance_tok(p);
            advance_tok(p);

            const char ** pnames = NULL;
            Type ** ptypes = NULL;
            bool * pnomds = NULL;
            int pc = 0, pcap = 8;
            pnames = (const char ** ) arena_alloc(a, sizeof(void * ) * 8);
            ptypes = (Type ** ) arena_alloc(a, sizeof(void * ) * 8);
            pnomds = (bool * ) arena_alloc(a, sizeof(void * ) * 8);
            bool variadic = false;
            const char * gparam = NULL;

            #define PUSH_PARAM(nm, ty, nd) do {
                \
                if (pc >= pcap) {
                    \
                    int _nc = pcap * 2;\
                    const char ** _nn = (const char ** ) arena_alloc(a, sizeof(void * ) * _nc);\
                    Type ** _nt = (Type ** ) arena_alloc(a, sizeof(void * ) * _nc);\
                    bool * _nb = (bool * ) arena_alloc(a, sizeof(void * ) * _nc);\
                    memcpy(_nn, pnames, sizeof(void * ) * (size_t) pc);\
                    memcpy(_nt, ptypes, sizeof(void * ) * (size_t) pc);\
                    memcpy(_nb, pnomds, sizeof(void * ) * (size_t) pc);\
                    pnames = _nn;
                    ptypes = _nt;
                    pnomds = _nb;
                    pcap = _nc;\
                }\
                pnames[pc] = (nm);
                ptypes[pc] = (ty);
                pnomds[pc] = (nd);
                pc++;\
            } while (0)

            while (!check(p, TOK_RPAREN) && !check(p, TOK_EOF)) {

                if (check(p, TOK_COMMA) && is_extern) {
                    advance_tok(p);
                    variadic = true;
                    break;
                }

                if (check(p, TOK_ARGV) || check(p, TOK_ARGC)) {
                    TokenType pk2 = p -> cur.type;
                    advance_tok(p);
                    const char * pname = p -> cur.sval ? arena_strdup(a, p -> cur.sval) : "?";
                    expect(p, TOK_IDENT, "parameter name");
                    Type * pty = make_type(a, pk2 == TOK_ARGV ? TY_PTR : TY_I32);
                    if (pk2 == TOK_ARGV) {
                        pty -> ptr.pointee = make_type(a, TY_STRING);
                    }
                    PUSH_PARAM(pname, pty, false);
                    if (!match_tok(p, TOK_COMMA)) break;
                    continue;
                }

                if (check(p, TOK_IDENT) && !is_type_start(p -> cur.type)) {
                    Token pk2 = lexer_peek(p -> lexer);
                    if (pk2.type == TOK_IDENT) {

                        gparam = arena_strdup(a, p -> cur.sval);
                        advance_tok(p);
                        const char * pname = arena_strdup(a, p -> cur.sval);
                        advance_tok(p);
                        Type * pty = make_type(a, TY_GENERIC);
                        pty -> generic.param = gparam;
                        PUSH_PARAM(pname, pty, false);
                        if (!match_tok(p, TOK_COMMA)) break;
                        continue;
                    }
                }
                bool pnomd = match_tok(p, TOK_NOMD);
                Type * pty = parse_type(p);
                const char * pname = p -> cur.sval ? arena_strdup(a, p -> cur.sval) : "?";
                expect(p, TOK_IDENT, "parameter name");
                PUSH_PARAM(pname, pty, pnomd);
                if (!match_tok(p, TOK_COMMA)) break;
            }
            #undef PUSH_PARAM
            expect(p, TOK_RPAREN, ")");

            Type * ret = NULL;

            if (check(p, TOK_LANGLE)) {
                ret = parse_angle_type(p);
            } else {
                ret = parse_type(p);
            }

            ASTNode * n = make_node(a, is_extern ? ND_EXTERN_DECL : ND_FUNC_DECL, ln);
            n -> func.name = fname;
            n -> func.param_names = pnames;
            n -> func.param_types = ptypes;
            n -> func.param_nomd = pnomds;
            n -> func.param_count = pc;
            n -> func.variadic = variadic;
            n -> func.ret_type = ret;
            n -> func.is_extern = is_extern;
            n -> func.is_unsafe = is_unsafe;
            n -> func.generic_param = gparam;

            if (is_extern) {
                expect(p, TOK_SEMICOLON, ";");
                n -> func.body = NULL;
            } else {
                n -> func.body = parse_block(p);
            }
            return n;
        }
    }

    {
        bool gnomd = match_tok(p, TOK_NOMD);
        Type * gtype = parse_type(p);
        const char * gname = p -> cur.sval ? arena_strdup(a, p -> cur.sval) : "?";
        expect(p, TOK_IDENT, "variable name");

        ASTNode * n = make_node(a, ND_GLOBAL_VAR, ln);
        n ->
            var.name = gname;
        n ->
            var.type = gtype;
        n ->
            var.nomd = gnomd;
        if (match_tok(p, TOK_EQ)) {
            n ->
                var.init = parse_expr(p);
        }
        expect(p, TOK_SEMICOLON, ";");
        return n;
    }
}

/* ================================================================
   PROGRAM PARSE
   ================================================================ */

static void inline_import(Parser * p,
    const char * import_path,
        ASTNode ** * items, int * cnt, int * cap) {
    Arena * a = p -> arena;

    char resolved[4096];
    {
        const char * base = g_source_path ? g_source_path : "";
        const char * ls = strrchr(base, '/');
        const char * bs = strrchr(base, '\\');
        const char * sep = (ls > bs) ? ls : bs;
        if (sep && import_path[0] != '/' &&
            !(import_path[0] && import_path[1] == ':')) {
            size_t dlen = (size_t)(sep - base + 1);
            snprintf(resolved, sizeof(resolved), "%.*s%s", (int) dlen, base, import_path);
        } else {
            snprintf(resolved, sizeof(resolved), "%s", import_path);
        }
    }

    for (int i = 0; i < g_imported_count; i++) {
        if (strcmp(g_imported_paths[i], resolved) == 0) return;
    }

    FILE * f = fopen(resolved, "rb");
    if (!f) {
        size_t rlen = strlen(resolved);
        char alt[4096];
        if (rlen > 3 && strcmp(resolved + rlen - 3, ".lc") == 0) {
            snprintf(alt, sizeof(alt), "%.*s.slc", (int)(rlen - 3), resolved);
            f = fopen(alt, "rb");
            if (f) snprintf(resolved, sizeof(resolved), "%s", alt);
        }
    }
    if (!f) {
        lc_error(p -> prev.line, p -> prev.col,
            "cannot open import '%s'", import_path);
        return;
    }

    if (g_imported_count < 1024)
        g_imported_paths[g_imported_count++] = arena_strdup(a, resolved);

    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    rewind(f);
    char * src = malloc((size_t) sz + 1);
    if (!src) {
        fclose(f);
        lc_error(0, 0, "out of memory reading import '%s'", import_path);
        return;
    }
    size_t r = fread(src, 1, (size_t) sz, f);
    src[r] = '\0';
    fclose(f);

    const char * saved_source = g_source;
    const char * saved_path = g_source_path;
    const char * resolved_path = arena_strdup(a, resolved);
    register_source_file(resolved_path, src);
    g_source = src;
    g_source_path = resolved_path;

    Lexer * lx = lexer_new(a, src);
    Parser * pp = parser_new(a, lx);
    ASTNode * sub = parse_program(pp);

    if (pp -> had_error) p -> had_error = true;

    g_source = saved_source;
    g_source_path = saved_path;

    for (int i = 0; i < sub -> program.count; i++)
        *
        items = (ASTNode ** ) vec_push(a, (void ** ) * items, cnt, cap, sub -> program.items[i]);
}

ASTNode * parse_program(Parser * p) {
    Arena * a = p -> arena;
    ASTNode * prog = make_node(a, ND_PROGRAM, 1);
    ASTNode ** items = NULL;
    int cnt = 0, cap = 0;

    while (!check(p, TOK_EOF)) {
        int ln = p -> cur.line;
        ASTNode * item = NULL;

        if (check(p, TOK_NOMAIN_DIR)) {
            advance_tok(p);
            match_tok(p, TOK_SEMICOLON);
            item = make_node(a, ND_NOMAIN, ln);
            items = (ASTNode ** ) vec_push(a, (void ** ) items, & cnt, & cap, item);
            continue;
        }
        if (check(p, TOK_AT)) {
            const char * dname = p -> cur.sval ? p -> cur.sval : "";
            advance_tok(p);
            expect(p, TOK_EQ, "=");
            if (!check(p, TOK_STRING_LIT)) {
                lc_error(p -> cur.line, p -> cur.col, "expected string literal after @%s=", dname);
                p -> had_error = true;
                p -> panic = true;
                synchronize(p);
                continue;
            }
            const char * dval = arena_strdup(a, p -> cur.sval);
            advance_tok(p);
            expect(p, TOK_SEMICOLON, ";");
            if (strcmp(dname, "tdl") == 0) {
                if (g_target_datalayout) {
                    lc_error(ln, 0, "multiple @tdl directives found");
                    p -> had_error = true;
                } else {
                    g_target_datalayout = dval;
                }
            } else if (strcmp(dname, "ttriple") == 0) {
                if (g_target_triple) {
                    lc_error(ln, 0, "multiple @ttriple directives found");
                    p -> had_error = true;
                } else {
                    g_target_triple = dval;
                }
            } else {
                lc_error(ln, 0, "unknown directive '@%s'", dname);
                p -> had_error = true;
            }
            continue;
        }

        if (check(p, TOK_IMPORT)) {
            advance_tok(p);
            do {
                const char * path;
                if (check(p, TOK_STRING_LIT)) {
                    path = arena_strdup(a, p -> cur.sval);
                    advance_tok(p);
                } else {
                    const char * bname = p -> cur.sval ? arena_strdup(a, p -> cur.sval) : "?";
                    expect(p, TOK_IDENT, "module name");
                    path = arena_sprintf(a, "%s.lc", bname);
                }
                inline_import(p, path, & items, & cnt, & cap);
            } while (match_tok(p, TOK_COMMA));
            expect(p, TOK_SEMICOLON, ";");
            if (p -> panic) synchronize(p);
            continue;
        }

        if (check(p, TOK_IN)) {
            advance_tok(p);
            const char * fname = p -> cur.sval ? arena_strdup(a, p -> cur.sval) : "?";
            expect(p, TOK_IDENT, "function name");
            expect(p, TOK_LBRACE, "{");
            expect(p, TOK_TYPECASE, "typecase");
            expect(p, TOK_LPAREN, "(");
            Type * tc_type = parse_type(p);
            expect(p, TOK_RPAREN, ")");

            const char * tc_param = NULL;
            if (check(p, TOK_IN) || (check(p, TOK_IDENT) && strcmp(p -> cur.sval, "for") == 0)) {
                advance_tok(p);
                expect(p, TOK_LPAREN, "(");
                tc_param = p -> cur.sval ? arena_strdup(a, p -> cur.sval) : "?";

                if (check(p, TOK_HASH)) {
                    advance_tok(p);
                    tc_param = "#";
                } else expect(p, TOK_IDENT, "param name");
                expect(p, TOK_RPAREN, ")");
            }
            ASTNode * body = parse_block(p);
            expect(p, TOK_RBRACE, "}");

            ASTNode * n = make_node(a, ND_IN_EXT, ln);
            n -> in_ext.func_name = fname;
            n -> in_ext.tc_type = tc_type;
            n -> in_ext.tc_param = tc_param;
            n -> in_ext.tc_body = body;
            item = n;
            items = (ASTNode ** ) vec_push(a, (void ** ) items, & cnt, & cap, item);
            continue;
        }

        bool is_unsafe = false, is_extern = false, is_packed = false;
        if (check(p, TOK_UNSAFE)) {
            advance_tok(p);
            is_unsafe = true;
        }
        if (check(p, TOK_EXTERN)) {
            advance_tok(p);
            is_extern = true;
        }
        if (check(p, TOK_PACKED)) {
            advance_tok(p);
            is_packed = true;
            if (check(p, TOK_STRUCT)) advance_tok(p);

        }

        if (is_packed) {
            item = parse_func_or_global(p, is_extern, is_unsafe, true);
        } else if (check(p, TOK_STRUCT) || check(p, TOK_ENUM) ||
            check(p, TOK_IDENT) ||
            is_type_start(p -> cur.type)) {
            item = parse_func_or_global(p, is_extern, is_unsafe, false);
        } else {
            lc_error(p -> cur.line, p -> cur.col,
                "unexpected token '%.*s' at top level",
                p -> cur.length, p -> cur.start);
            p -> had_error = true;
            p -> panic = true;
        }

        if (p -> panic) {
            synchronize(p);
            continue;
        }
        if (item) items = (ASTNode ** ) vec_push(a, (void ** ) items, & cnt, & cap, item);
    }

    prog -> program.items = items;
    prog -> program.count = cnt;
    return prog;
}

/* ================================================================
   KEYWORD TABLE
   ================================================================ */

typedef struct {
    const char * word;
    TokenType type;
}
Keyword;

static Keyword keywords[] = {
    {
        "ret",
        TOK_RET
    },
    {
        "return",
        TOK_RET
    },
    {
        "if",
        TOK_IF
    },
    {
        "else",
        TOK_ELSE
    },
    {
        "loop",
        TOK_LOOP
    },
    {
        "struct",
        TOK_STRUCT
    },
    {
        "packed",
        TOK_PACKED
    },
    {
        "enum",
        TOK_ENUM
    },
    {
        "match",
        TOK_MATCH
    },
    {
        "extern",
        TOK_EXTERN
    },
    {
        "unsafe",
        TOK_UNSAFE
    },
    {
        "import",
        TOK_IMPORT
    },
    {
        "in",
        TOK_IN
    },
    {
        "typeswitch",
        TOK_TYPESWITCH
    },
    {
        "typecase",
        TOK_TYPECASE
    },
    {
        "fallback",
        TOK_FALLBACK
    },
    {
        "nomd",
        TOK_NOMD
    },
    {
        "crumble",
        TOK_CRUMBLE
    },
    {
        "dropall",
        TOK_DROPALL
    },
    {
        "readonly",
        TOK_READONLY
    },
    {
        "writeonly",
        TOK_WRITEONLY
    },
    {
        "br",
        TOK_BR
    },
    {
        "cont",
        TOK_CONT
    },
    {
        "argv",
        TOK_ARGV
    },
    {
        "argc",
        TOK_ARGC
    },
    {
        "true",
        TOK_TRUE
    },
    {
        "false",
        TOK_FALSE
    },
    {
        "i8",
        TOK_I8
    },
    {
        "i16",
        TOK_I16
    },
    {
        "i32",
        TOK_I32
    },
    {
        "i64",
        TOK_I64
    },
    {
        "u8",
        TOK_U8
    },
    {
        "u16",
        TOK_U16
    },
    {
        "u32",
        TOK_U32
    },
    {
        "u64",
        TOK_U64
    },
    {
        "f32",
        TOK_F32
    },
    {
        "f64",
        TOK_F64
    },
    {
        "str",
        TOK_STRING_KW
    },
    {
        "char",
        TOK_CHAR_KW
    },
    {
        "bool",
        TOK_BOOL_KW
    },
    {
        "void",
        TOK_VOID_KW
    },
    {
        "int",
        TOK_INT_KW
    },
    {
        "float",
        TOK_FLOAT_KW
    },
    {
        NULL,
        TOK_ERROR
    }
};

/* ================================================================
   LEXER
   ================================================================ */

Lexer * lexer_new(Arena * a,
    const char * src) {
    Lexer * l = arena_alloc(a, sizeof(Lexer));
    l -> src = src;
    l -> pos = 0;
    l -> line = 1;
    l -> col = 1;
    l -> arena = a;
    l -> has_peeked = false;
    return l;
}

static char cur_char(Lexer * l) {
    return l -> src[l -> pos];
}
static char peek_char(Lexer * l) {
    return l -> src[l -> pos + 1];
}

static char advance(Lexer * l) {
    char c = l -> src[l -> pos++];
    if (c == '\n') {
        l -> line++;
        l -> col = 1;
    } else {
        l -> col++;
    }
    return c;
}

static bool at_end(Lexer * l) {
    return l -> src[l -> pos] == '\0';
}

static void skip_ws(Lexer * l) {
    again: while (!at_end(l) && isspace((unsigned char) cur_char(l)))
        advance(l);

    if (!at_end(l) && cur_char(l) == '/' && peek_char(l) == '/') {
        while (!at_end(l) && cur_char(l) != '\n') advance(l);
        goto again;
    }

    if (!at_end(l) && cur_char(l) == '/' && peek_char(l) == '*') {
        int start_line = l -> line;
        advance(l);
        advance(l);
        int depth = 1;
        while (!at_end(l) && depth > 0) {
            if (cur_char(l) == '/' && peek_char(l) == '*') {
                advance(l);
                advance(l);
                depth++;
            } else if (cur_char(l) == '*' && peek_char(l) == '/') {
                advance(l);
                advance(l);
                depth--;
            } else {
                advance(l);
            }
        }
        if (depth != 0)
            lc_error(start_line, 0, "unterminated block comment");
        goto again;
    }
}

static Token lex_number(Lexer * l) {
    Token t;
    t.line = l -> line;
    t.col = l -> col;
    t.start = l -> src + l -> pos;
    bool is_hex = false;
    bool is_float = false;

    if (cur_char(l) == '0' && (peek_char(l) == 'x' || peek_char(l) == 'X')) {
        is_hex = true;
        advance(l);
        advance(l);
        while (!at_end(l) && isxdigit((unsigned char) cur_char(l))) advance(l);
    } else {
        while (!at_end(l) && isdigit((unsigned char) cur_char(l))) advance(l);
        if (!at_end(l) && cur_char(l) == '.' && isdigit((unsigned char) peek_char(l))) {
            is_float = true;
            advance(l);
            while (!at_end(l) && isdigit((unsigned char) cur_char(l))) advance(l);
        }
        if (!at_end(l) && (cur_char(l) == 'e' || cur_char(l) == 'E')) {
            is_float = true;
            advance(l);
            if (!at_end(l) && (cur_char(l) == '+' || cur_char(l) == '-')) advance(l);
            while (!at_end(l) && isdigit((unsigned char) cur_char(l))) advance(l);
        }
    }

    t.length = (int)(l -> src + l -> pos - t.start);
    char buf[128];
    int n = t.length < 127 ? t.length : 127;
    memcpy(buf, t.start, n);
    buf[n] = '\0';

    if (is_float) {
        t.type = TOK_FLOAT_LIT;
        t.fval = atof(buf);
    } else {
        t.type = TOK_INT_LIT;
        t.ival = is_hex ? (int64_t) strtoll(buf, NULL, 16) :
            (int64_t) strtoll(buf, NULL, 10);
    }
    return t;
}

static char parse_escape(Lexer * l) {
    advance(l);
    char c = advance(l);
    switch (c) {
    case 'n':
        return '\n';
    case 't':
        return '\t';
    case 'r':
        return '\r';
    case '0':
        return '\0';
    case '\\':
        return '\\';
    case '\'':
        return '\'';
    case '"':
        return '"';
    case '{':
        return '{';
    case '}':
        return '}';
    default:
        lc_error(l -> line, l -> col, "unknown escape '\\%c'", c);
        return c;
    }
}

static Token lex_string(Lexer * l) {
    Token t;
    t.line = l -> line;
    t.col = l -> col;
    advance(l);
    char buf[8192];
    int len = 0;
    bool has_fmt = false;
    while (!at_end(l) && cur_char(l) != '"') {
        if (cur_char(l) == '\\') {
            buf[len++] = parse_escape(l);
        } else {
            if (cur_char(l) == '{') has_fmt = true;
            buf[len++] = advance(l);
        }
        if (len >= 8190) {
            lc_error(t.line, t.col, "string literal too long");
            break;
        }
    }
    if (at_end(l)) lc_error(t.line, t.col, "unterminated string literal");
    else advance(l);
    buf[len] = '\0';
    t.type = has_fmt ? TOK_STRING_LIT : TOK_STRING_LIT;
    t.sval = arena_strdup(l -> arena, buf);
    t.length = len;

    (void) has_fmt;
    return t;
}

static Token lex_char(Lexer * l) {
    Token t;
    t.line = l -> line;
    t.col = l -> col;
    advance(l);
    char c;
    if (cur_char(l) == '\\') {
        c = parse_escape(l);
    } else {
        c = advance(l);
    }
    if (cur_char(l) != '\'')
        lc_error(t.line, t.col, "unterminated char literal");
    else advance(l);
    t.type = TOK_CHAR_LIT;
    t.ival = (int64_t)(unsigned char) c;
    t.length = 1;
    return t;
}

static TokenType classify_ident(const char * s, int len) {
    char buf[64];
    if (len >= 64) return TOK_IDENT;
    memcpy(buf, s, (size_t) len);
    buf[len] = '\0';
    for (int i = 0; keywords[i].word; i++)
        if (strcmp(keywords[i].word, buf) == 0)
            return keywords[i].type;
    return TOK_IDENT;
}

Token lexer_next(Lexer * l) {
    if (l -> has_peeked) {
        l -> has_peeked = false;
        return l -> peeked;
    }

    skip_ws(l);

    if (at_end(l)) {
        Token t;
        t.type = TOK_EOF;
        t.line = l -> line;
        t.col = l -> col;
        t.start = l -> src + l -> pos;
        t.length = 0;
        return t;
    }

    Token t;
    t.line = l -> line;
    t.col = l -> col;
    t.start = l -> src + l -> pos;
    t.length = 1;
    t.ival = 0;

    char c = cur_char(l);

    if (c == '@') {
        advance(l);
        if (strncmp(l -> src + l -> pos, "nomain", 6) == 0 &&
            !isalnum((unsigned char) l -> src[l -> pos + 6]) &&
            l -> src[l -> pos + 6] != '_') {
            for (int i = 0; i < 6; i++) advance(l);
            t.type = TOK_NOMAIN_DIR;
            t.length = (int)(l -> src + l -> pos - t.start);
            return t;
        }
        if (isalpha((unsigned char) cur_char(l)) || cur_char(l) == '_') {
            const char * ds = l -> src + l -> pos;
            while (!at_end(l) && (isalnum((unsigned char) cur_char(l)) || cur_char(l) == '_'))
                advance(l);
            int dlen = (int)(l -> src + l -> pos - ds);
            t.type = TOK_AT;
            t.sval = arena_strndup(l -> arena, ds, dlen);
            t.length = (int)(l -> src + l -> pos - t.start);
            return t;
        }
        t.type = TOK_AT;
        t.length = (int)(l -> src + l -> pos - t.start);
        return t;
    }

    if (isdigit((unsigned char) c)) return lex_number(l);

    if (c == '"') return lex_string(l);

    if (c == '\'') return lex_char(l);

    if (isalpha((unsigned char) c) || c == '_') {
        while (!at_end(l) && (isalnum((unsigned char) cur_char(l)) || cur_char(l) == '_'))
            advance(l);
        int len = (int)(l -> src + l -> pos - t.start);
        t.length = len;
        t.type = classify_ident(t.start, len);
        if (t.type == TOK_IDENT)
            t.sval = arena_strndup(l -> arena, t.start, len);
        return t;
    }

    advance(l);
    char n = cur_char(l);

    #define TWO(a, b, tok) if (c == (a) && n == (b)) {
        advance(l);
        t.type = (tok);
        t.length = 2;
        return t;
    }
    TWO('-', '>', TOK_ARROW)
    TWO('=', '=', TOK_EQEQ)
    TWO('!', '=', TOK_NEQ)
    TWO('<', '<', TOK_LSHIFT)
    TWO('>', '>', TOK_RSHIFT)
    TWO('<', '=', TOK_LTE)
    TWO('>', '=', TOK_GTE)
    TWO('&', '&', TOK_ANDAND)
    TWO('|', '|', TOK_OROR)
    TWO('+', '=', TOK_PLUSEQ)
    TWO('-', '=', TOK_MINUSEQ)
    TWO('*', '=', TOK_STAREQ)
    TWO('/', '=', TOK_SLASHEQ)
    TWO('%', '=', TOK_PERCENTEQ)
    TWO('&', '=', TOK_AMPEQ)
    TWO('|', '=', TOK_PIPEEQ)
    TWO('^', '=', TOK_CARETEQ)
    #undef TWO

    if (c == '<' && n == '<' && l -> src[l -> pos + 1] == '=') {
        advance(l);
        advance(l);
        t.type = TOK_LSHIFTEQ;
        t.length = 3;
        return t;
    }
    if (c == '>' && n == '>' && l -> src[l -> pos + 1] == '=') {
        advance(l);
        advance(l);
        t.type = TOK_RSHIFTEQ;
        t.length = 3;
        return t;
    }

    switch (c) {
    case '(':
        t.type = TOK_LPAREN;
        break;
    case ')':
        t.type = TOK_RPAREN;
        break;
    case '{':
        t.type = TOK_LBRACE;
        break;
    case '}':
        t.type = TOK_RBRACE;
        break;
    case '[':
        t.type = TOK_LBRACKET;
        break;
    case ']':
        t.type = TOK_RBRACKET;
        break;
    case ';':
        t.type = TOK_SEMICOLON;
        break;
    case ',':
        t.type = TOK_COMMA;
        break;
    case '.':
        t.type = TOK_DOT;
        break;
    case '#':
        t.type = TOK_HASH;
        break;
    case '?':
        t.type = TOK_QUESTION;
        break;
    case ':':
        t.type = TOK_COLON;
        break;
    case '<':
        t.type = TOK_LANGLE;
        break;
    case '>':
        t.type = TOK_RANGLE;
        break;
    case '+':
        t.type = TOK_PLUS;
        break;
    case '-':
        t.type = TOK_MINUS;
        break;
    case '*':
        t.type = TOK_STAR;
        break;
    case '/':
        t.type = TOK_SLASH;
        break;
    case '%':
        t.type = TOK_PERCENT;
        break;
    case '=':
        t.type = TOK_EQ;
        break;
    case '!':
        t.type = TOK_BANG;
        break;
    case '&':
        t.type = TOK_AMP;
        break;
    case '|':
        t.type = TOK_PIPE;
        break;
    case '^':
        t.type = TOK_CARET;
        break;
    case '~':
        t.type = TOK_TILDE;
        break;
    default:
        lc_error(t.line, t.col, "unexpected character '%c'", c);
        t.type = TOK_ERROR;
        break;
    }
    return t;
}

Token lexer_peek(Lexer * l) {
    if (!l -> has_peeked) {
        l -> peeked = lexer_next(l);
        l -> has_peeked = true;
    }
    return l -> peeked;
}

void lexer_consume(Lexer * l) {
    if (l -> has_peeked) l -> has_peeked = false;
    else lexer_next(l);
}

const char * token_type_name(TokenType t) {
    switch (t) {
        #define N(x) case x: return #x;
        N(TOK_INT_LIT) N(TOK_FLOAT_LIT) N(TOK_STRING_LIT) N(TOK_CHAR_LIT)
        N(TOK_TRUE) N(TOK_FALSE) N(TOK_IDENT)
        N(TOK_RET) N(TOK_IF) N(TOK_ELSE) N(TOK_LOOP)
        N(TOK_STRUCT) N(TOK_PACKED) N(TOK_ENUM) N(TOK_MATCH)
        N(TOK_EXTERN) N(TOK_UNSAFE) N(TOK_IMPORT) N(TOK_IN)
        N(TOK_TYPESWITCH) N(TOK_TYPECASE) N(TOK_FALLBACK)
        N(TOK_CRUMBLE) N(TOK_DROPALL) N(TOK_READONLY) N(TOK_WRITEONLY)
        N(TOK_BR) N(TOK_CONT)
        N(TOK_LPAREN) N(TOK_RPAREN) N(TOK_LBRACE) N(TOK_RBRACE)
        N(TOK_LBRACKET) N(TOK_RBRACKET) N(TOK_SEMICOLON) N(TOK_COMMA)
        N(TOK_DOT) N(TOK_ARROW) N(TOK_HASH) N(TOK_QUESTION)
        N(TOK_EQ) N(TOK_EQEQ) N(TOK_NEQ)
        N(TOK_PLUS) N(TOK_MINUS) N(TOK_STAR) N(TOK_SLASH) N(TOK_PERCENT)
        N(TOK_LTE) N(TOK_GTE) N(TOK_LANGLE) N(TOK_RANGLE)
        N(TOK_ANDAND) N(TOK_OROR) N(TOK_BANG)
        N(TOK_AMP) N(TOK_PIPE) N(TOK_CARET) N(TOK_TILDE)
        N(TOK_LSHIFT) N(TOK_RSHIFT)
        N(TOK_PLUSEQ) N(TOK_MINUSEQ) N(TOK_STAREQ) N(TOK_SLASHEQ)
        N(TOK_PERCENTEQ) N(TOK_AMPEQ) N(TOK_PIPEEQ) N(TOK_CARETEQ)
        N(TOK_LSHIFTEQ) N(TOK_RSHIFTEQ)
        N(TOK_NOMAIN_DIR) N(TOK_EOF) N(TOK_ERROR)
        #undef N
    default:
        return "?";
    }
}

/* ================================================================
   CRUMB CHECKER
   Tracks read/write limits per variable and enforces them at
   compile time. Also flags unlimited usage with warnings.
   ================================================================ */

typedef struct CrumbInfo {
    const char * name;
    int reads_left;
    int writes_left;
    int reads_used;
    int writes_used;
    int decl_line;
    struct CrumbInfo * next;
}
CrumbInfo;

typedef struct CrumbScope {
    CrumbInfo * head;
    struct CrumbScope * parent;
}
CrumbScope;

typedef struct {
    CrumbScope * scope;
    bool had_error;
    bool suppress_warn;
    Arena * arena;
}
CC;

static CrumbScope * crumb_push(CC * cc) {
    CrumbScope * s = arena_alloc(cc -> arena, sizeof(CrumbScope));
    s -> parent = cc -> scope;
    cc -> scope = s;
    return s;
}

static void crumb_pop(CC * cc) {
    CrumbScope * s = cc -> scope;

    if (!cc -> suppress_warn) {
        for (CrumbInfo * e = s -> head; e; e = e -> next) {
            if (e -> reads_left == -1 && e -> reads_used > 0)
                lc_warn(e -> decl_line,
                    "crumb: '%s' had unlimited reads (%d detected); consider crumble(!r=%d)",
                    e -> name, e -> reads_used, e -> reads_used);
            if (e -> writes_left == -1 && e -> writes_used > 0)
                lc_warn(e -> decl_line,
                    "crumb: '%s' had unlimited writes (%d detected); consider crumble(!w=%d)",
                    e -> name, e -> writes_used, e -> writes_used);
        }
    }
    cc -> scope = s -> parent;
}

static CrumbInfo * crumb_find(CC * cc,
    const char * name) {
    for (CrumbScope * s = cc -> scope; s; s = s -> parent)
        for (CrumbInfo * e = s -> head; e; e = e -> next)
            if (strcmp(e -> name, name) == 0) return e;
    return NULL;
}

static void crumb_declare(CC * cc,
    const char * name, int line) {
    CrumbInfo * e = arena_alloc(cc -> arena, sizeof(CrumbInfo));
    e -> name = name;
    e -> reads_left = -1;
    e -> writes_left = -1;
    e -> reads_used = 0;
    e -> writes_used = 0;
    e -> decl_line = line;
    e -> next = cc -> scope -> head;
    cc -> scope -> head = e;
}

static void crumb_apply(CC * cc, ASTNode * crumble_node) {
    for (int i = 0; i < crumble_node -> crumble.var_count; i++) {
        const char * name = crumble_node -> crumble.vars[i];
        CrumbInfo * e = crumb_find(cc, name);
        if (!e) {
            lc_error(crumble_node -> line, 0,
                "crumb: variable '%s' not found in scope", name);
            cc -> had_error = true;
            continue;
        }
        if (crumble_node -> crumble.read_limit >= 0)
            e -> reads_left = crumble_node -> crumble.read_limit;
        if (crumble_node -> crumble.write_limit >= 0)
            e -> writes_left = crumble_node -> crumble.write_limit;
    }
}

static void crumb_read(CC * cc,
    const char * name, int line) {
    CrumbInfo * e = crumb_find(cc, name);
    if (!e) return;
    if (e -> reads_left == 0) {
        lc_error(line, 0, "crumb: '%s' has no reads remaining (exhausted)", name);
        cc -> had_error = true;
        return;
    }
    if (e -> reads_left > 0) e -> reads_left--;
    e -> reads_used++;
}

static void crumb_write(CC * cc,
    const char * name, int line) {
    CrumbInfo * e = crumb_find(cc, name);
    if (!e) return;
    if (e -> writes_left == 0) {
        lc_error(line, 0, "crumb: '%s' has no writes remaining (exhausted)", name);
        cc -> had_error = true;
        return;
    }
    if (e -> writes_left > 0) e -> writes_left--;
    e -> writes_used++;
}

static void check_expr(CC * cc, ASTNode * n);
static void check_stmt(CC * cc, ASTNode * n);

static void check_expr(CC * cc, ASTNode * n) {
    if (!n) return;
    switch (n -> kind) {
    case ND_IDENT:
        crumb_read(cc, n -> ident.name, n -> line);
        break;
    case ND_ASSIGN:

        if (n -> assign.lhs -> kind == ND_IDENT)
            crumb_write(cc, n -> assign.lhs -> ident.name, n -> line);
        else if (n -> assign.lhs -> kind == ND_INDEX)
            check_expr(cc, n -> assign.lhs -> idx.array);
        check_expr(cc, n -> assign.rhs);
        break;
    case ND_BINARY:
        check_expr(cc, n -> binary.lhs);
        check_expr(cc, n -> binary.rhs);
        break;
    case ND_UNARY:
        check_expr(cc, n -> unary.operand);
        break;
    case ND_CAST:
        check_expr(cc, n -> cast.operand);
        break;
    case ND_CALL:
        check_expr(cc, n -> call.callee);
        for (int i = 0; i < n -> call.arg_count; i++)
            check_expr(cc, n -> call.args[i]);
        break;
    case ND_INDEX:
        check_expr(cc, n -> idx.array);
        check_expr(cc, n -> idx.index);
        break;
    case ND_FIELD:
        check_expr(cc, n -> field.object);
        break;
    case ND_ENUM_FIELD:
        for (int i = 0; i < n -> enum_field.arg_count; i++)
            check_expr(cc, n -> enum_field.args[i]);
        break;
    case ND_TERNARY:
        check_expr(cc, n -> ternary.cond);
        check_expr(cc, n -> ternary.then_val);
        check_expr(cc, n -> ternary.else_val);
        break;
    case ND_ARRAY_LIT:
        for (int i = 0; i < n -> array_lit.count; i++)
            check_expr(cc, n -> array_lit.elems[i]);
        break;
    case ND_STRUCT_INIT:
        for (int i = 0; i < n -> struct_init.field_count; i++)
            check_expr(cc, n -> struct_init.field_vals[i]);
        break;
    default:
        break;
    }
}

static void check_stmt(CC * cc, ASTNode * n) {
    if (!n) return;
    switch (n -> kind) {
    case ND_BLOCK:
        crumb_push(cc);
        for (int i = 0; i < n -> block.count; i++)
            check_stmt(cc, n -> block.stmts[i]);
        crumb_pop(cc);
        break;
    case ND_VAR_DECL:
        if (n ->
            var.init) check_expr(cc, n ->
            var.init);
        crumb_declare(cc, n ->
            var.name, n -> line);
        break;
    case ND_CRUMBLE:
        crumb_apply(cc, n);
        break;
    case ND_ASSIGN:
        check_expr(cc, n);
        break;
    case ND_RETURN:
        if (n -> ret.val) check_expr(cc, n -> ret.val);
        break;
    case ND_IF:
        check_expr(cc, n -> ifstmt.cond);
        check_stmt(cc, n -> ifstmt.then_body);
        if (n -> ifstmt.else_body) check_stmt(cc, n -> ifstmt.else_body);
        break;
    case ND_LOOP:
        crumb_push(cc);
        if (n -> loop.counter) check_stmt(cc, n -> loop.counter);
        if (n -> loop.cond) check_expr(cc, n -> loop.cond);
        if (n -> loop.step) check_expr(cc, n -> loop.step);
        check_stmt(cc, n -> loop.body);
        crumb_pop(cc);
        break;
    case ND_MATCH:
        check_expr(cc, n -> match.subject);
        for (int i = 0; i < n -> match.arm_count; i++) {
            crumb_push(cc);
            ASTNode * arm = n -> match.arms[i];
            if (arm -> match_arm.bind_name)
                crumb_declare(cc, arm -> match_arm.bind_name, arm -> line);
            check_stmt(cc, arm -> match_arm.body);
            crumb_pop(cc);
        }
        break;
    case ND_TYPESWITCH: {
        bool saved = cc -> suppress_warn;
        cc -> suppress_warn = true;
        for (int i = 0; i < n -> typeswitch.case_count; i++)
            check_stmt(cc, n -> typeswitch.cases[i] -> typecase.body);
        if (n -> typeswitch.fallback)
            check_stmt(cc, n -> typeswitch.fallback);
        cc -> suppress_warn = saved;
        break;
    }
    case ND_EXPR_STMT:
        check_expr(cc, n -> expr_stmt.expr);
        break;
    default:
        break;
    }
}

void crumb_check(ASTNode * program) {
    CC cc;
    cc.had_error = false;
    cc.arena = g_arena;
    cc.scope = NULL;

    crumb_push( & cc);

    for (int i = 0; i < program -> program.count; i++) {
        ASTNode * item = program -> program.items[i];
        switch (item -> kind) {
        case ND_GLOBAL_VAR:
            crumb_declare( & cc, item ->
                var.name, item -> line);
            if (item ->
                var.init) check_expr( & cc, item ->
                var.init);
            break;
        case ND_FUNC_DECL: {

            if (item -> func.generic_param) break;
            crumb_push( & cc);
            for (int j = 0; j < item -> func.param_count; j++)
                crumb_declare( & cc, item -> func.param_names[j], item -> line);
            if (item -> func.body)
                check_stmt( & cc, item -> func.body);
            crumb_pop( & cc);
            break;
        }
        default:
            break;
        }
    }

    crumb_pop( & cc);
    if (cc.had_error) g_errors++;
}

/* ================================================================
   SEMANTIC CHECKER
   - catches duplicate definitions
   - checks type compatibility
   ================================================================ */

typedef struct SemEntry {
    const char * name;
    Type * type;
    int line;
    struct SemEntry * next;
}
SemEntry;

typedef struct SemScope {
    SemEntry * entries;
    struct SemScope * parent;
}
SemScope;

typedef struct {
    ASTNode * program;
    Arena * arena;
    SemScope * scope;
    Type * cur_ret;
    const char * cur_generic;
    const char * source_path;
    bool had_error;
}
SemCtx;

static bool sem_is_int_kind(TypeKind k) {
    switch (k) {
    case TY_BOOL:
    case TY_I8:
    case TY_I16:
    case TY_I32:
    case TY_I64:
    case TY_U8:
    case TY_U16:
    case TY_U32:
    case TY_U64:
    case TY_CHAR:
    case TY_INT_GENERIC:
        return true;
    default:
        return false;
    }
}

static bool sem_is_float_kind(TypeKind k) {
    return k == TY_F32 || k == TY_F64 || k == TY_FLOAT_GENERIC;
}

static bool sem_is_num_kind(TypeKind k) {
    return sem_is_int_kind(k) || sem_is_float_kind(k);
}

static ASTNode * sem_find_struct(ASTNode * program,
    const char * name) {
    for (int i = 0; i < program -> program.count; i++) {
        ASTNode * it = program -> program.items[i];
        if (it -> kind == ND_STRUCT_DECL && strcmp(it -> strct.name, name) == 0)
            return it;
    }
    return NULL;
}

static ASTNode * sem_find_enum(ASTNode * program,
    const char * name) {
    for (int i = 0; i < program -> program.count; i++) {
        ASTNode * it = program -> program.items[i];
        if (it -> kind == ND_ENUM_DECL && strcmp(it -> enm.name, name) == 0)
            return it;
    }
    return NULL;
}

static ASTNode * sem_find_func(ASTNode * program,
    const char * name) {
    for (int i = 0; i < program -> program.count; i++) {
        ASTNode * it = program -> program.items[i];
        if ((it -> kind == ND_FUNC_DECL || it -> kind == ND_EXTERN_DECL) &&
            strcmp(it -> func.name, name) == 0)
            return it;
    }
    return NULL;
}

static SemScope * sem_push(SemCtx * sc) {
    SemScope * s = arena_alloc(sc -> arena, sizeof(SemScope));
    s -> parent = sc -> scope;
    s -> entries = NULL;
    sc -> scope = s;
    return s;
}

static void sem_pop(SemCtx * sc) {
    sc -> scope = sc -> scope ? sc -> scope -> parent : NULL;
}

static SemEntry * sem_lookup_local(SemCtx * sc,
    const char * name) {
    if (!sc -> scope) return NULL;
    for (SemEntry * e = sc -> scope -> entries; e; e = e -> next)
        if (strcmp(e -> name, name) == 0) return e;
    return NULL;
}

static SemEntry * sem_lookup(SemCtx * sc,
    const char * name) {
    for (SemScope * s = sc -> scope; s; s = s -> parent)
        for (SemEntry * e = s -> entries; e; e = e -> next)
            if (strcmp(e -> name, name) == 0) return e;
    return NULL;
}

static void sem_define(SemCtx * sc,
    const char * name, Type * ty, int line) {
    if (sem_lookup_local(sc, name)) {
        lc_error_tok_at(sc -> source_path, line, source_first_content_col_at(sc -> source_path, line), source_content_len_at(sc -> source_path, line), "duplicate definition of '%s'", name);
        sc -> had_error = true;
        return;
    }
    SemEntry * e = arena_alloc(sc -> arena, sizeof(SemEntry));
    e -> name = name;
    e -> type = ty;
    e -> line = line;
    e -> next = sc -> scope -> entries;
    sc -> scope -> entries = e;
}

static bool sem_types_compatible(Type * src, Type * dst) {
    if (!src || !dst) return false;
    if (src -> kind == TY_GENERIC || dst -> kind == TY_GENERIC) return true;
    if (types_equal(src, dst)) return true;

    if (src -> kind == TY_CONTEXT || dst -> kind == TY_CONTEXT) return true;
    if (src -> kind == TY_GENERIC || dst -> kind == TY_GENERIC) return true;

    if (src -> kind == TY_INT_GENERIC && sem_is_int_kind(dst -> kind)) return true;
    if (dst -> kind == TY_INT_GENERIC && sem_is_int_kind(src -> kind)) return true;
    if (src -> kind == TY_FLOAT_GENERIC && sem_is_float_kind(dst -> kind)) return true;
    if (dst -> kind == TY_FLOAT_GENERIC && sem_is_float_kind(src -> kind)) return true;

    if (sem_is_int_kind(src -> kind) && sem_is_int_kind(dst -> kind)) return true;
    if (sem_is_float_kind(src -> kind) && sem_is_float_kind(dst -> kind)) return true;

    if (src -> kind == TY_PTR && dst -> kind == TY_PTR) return true;
    if (src -> kind == TY_STRING && dst -> kind == TY_STRING) return true;

    if (src -> kind == TY_ARRAY && dst -> kind == TY_ARRAY) {
        return src -> array.count == dst -> array.count &&
            sem_types_compatible(src -> array.elem, dst -> array.elem);
    }

    if (src -> kind == TY_STRUCT && dst -> kind == TY_STRUCT)
        return strcmp(src -> named.name, dst -> named.name) == 0;
    if (src -> kind == TY_ENUM && dst -> kind == TY_ENUM)
        return strcmp(src -> named.name, dst -> named.name) == 0;

    if (src -> kind == TY_FUNC && dst -> kind == TY_FUNC) return true;
    return false;
}

static Type * sem_normalize_generic_type(Type * ty,
    const char * generic_name) {
    if (!ty || !generic_name) return ty;
    switch (ty -> kind) {
    case TY_STRUCT:
    case TY_ENUM:
        if (strcmp(ty -> named.name, generic_name) == 0) {
            ty -> kind = TY_GENERIC;
            ty -> generic.param = generic_name;
        }
        return ty;
    case TY_ARRAY:
        ty -> array.elem = sem_normalize_generic_type(ty -> array.elem, generic_name);
        return ty;
    case TY_PTR:
        ty -> ptr.pointee = sem_normalize_generic_type(ty -> ptr.pointee, generic_name);
        return ty;
    case TY_FUNC:
        for (int i = 0; i < ty -> func.param_count; i++)
            ty -> func.params[i] = sem_normalize_generic_type(ty -> func.params[i], generic_name);
        ty -> func.ret = sem_normalize_generic_type(ty -> func.ret, generic_name);
        return ty;
    default:
        return ty;
    }
}

static bool sem_type_known(SemCtx * sc, Type * ty,
    const char * generic_name) {
    if (!ty) return true;
    switch (ty -> kind) {
    case TY_VOID:
    case TY_BOOL:
    case TY_I8:
    case TY_I16:
    case TY_I32:
    case TY_I64:
    case TY_U8:
    case TY_U16:
    case TY_U32:
    case TY_U64:
    case TY_F32:
    case TY_F64:
    case TY_STRING:
    case TY_CHAR:
    case TY_INT_GENERIC:
    case TY_FLOAT_GENERIC:
    case TY_GENERIC:
    case TY_CONTEXT:
        return true;
    case TY_ARRAY:
        return sem_type_known(sc, ty -> array.elem, generic_name);
    case TY_PTR:
        return sem_type_known(sc, ty -> ptr.pointee, generic_name);
    case TY_STRUCT:
        if (generic_name && strcmp(ty -> named.name, generic_name) == 0) return true;
        return sem_find_struct(sc -> program, ty -> named.name) != NULL;
    case TY_ENUM:
        if (generic_name && strcmp(ty -> named.name, generic_name) == 0) return true;
        return sem_find_enum(sc -> program, ty -> named.name) != NULL;
    case TY_FUNC:
        for (int i = 0; i < ty -> func.param_count; i++)
            if (!sem_type_known(sc, ty -> func.params[i], generic_name)) return false;
        return sem_type_known(sc, ty -> func.ret, generic_name);
    default:
        return false;
    }
}

static void sem_report_unknown_type(SemCtx * sc,
    const char * name, int line) {
    lc_error_at(sc -> source_path, line,
        source_ident_col_at(sc -> source_path, line, name),
        "unknown type '%s'", name);
    sc -> had_error = true;
}

static Type * sem_struct_field_type(SemCtx * sc,
    const char * struct_name,
        const char * field, int line) {
    ASTNode * sd = sem_find_struct(sc -> program, struct_name);
    if (!sd) {
        lc_error_at(sc -> source_path, line, 0, "unknown struct '%s'", struct_name);
        sc -> had_error = true;
        return make_type(sc -> arena, TY_VOID);
    }
    for (int i = 0; i < sd -> strct.field_count; i++) {
        if (strcmp(sd -> strct.field_names[i], field) == 0)
            return sd -> strct.field_types[i];
    }
    lc_error_at(sc -> source_path, line, 0, "struct '%s' has no field '%s'", struct_name, field);
    sc -> had_error = true;
    return make_type(sc -> arena, TY_VOID);
}

static Type * sem_enum_variant_type(SemCtx * sc,
    const char * enum_name,
        const char * variant,
            int * variant_index, int line) {
    ASTNode * ed = sem_find_enum(sc -> program, enum_name);
    if (!ed) {
        lc_error_at(sc -> source_path, line, 0, "unknown enum '%s'", enum_name);
        sc -> had_error = true;
        if (variant_index) * variant_index = -1;
        return NULL;
    }
    for (int i = 0; i < ed -> enm.variant_count; i++) {
        if (strcmp(ed -> enm.variant_names[i], variant) == 0) {
            if (variant_index) * variant_index = i;
            return ed -> enm.variant_types[i];
        }
    }
    lc_error_at(sc -> source_path, line, 0, "enum '%s' has no variant '%s'", enum_name, variant);
    sc -> had_error = true;
    if (variant_index) * variant_index = -1;
    return NULL;
}

static Type * sem_infer_expr(SemCtx * sc, ASTNode * n);
static Type * sem_infer_lvalue(SemCtx * sc, ASTNode * n);
static void sem_check_stmt(SemCtx * sc, ASTNode * n);

static Type * sem_infer_lvalue(SemCtx * sc, ASTNode * n) {
    if (!n) return make_type(sc -> arena, TY_VOID);
    switch (n -> kind) {
    case ND_IDENT: {
        SemEntry * e = sem_lookup(sc, n -> ident.name);
        if (!e) {
            lc_error_at(sc -> source_path, n -> line, 0, "undefined variable '%s'", n -> ident.name);
            sc -> had_error = true;
            return make_type(sc -> arena, TY_VOID);
        }
        return e -> type;
    }
    case ND_FIELD: {
        Type * obj = sem_infer_expr(sc, n -> field.object);
        if (!obj) return make_type(sc -> arena, TY_VOID);
        if (obj -> kind == TY_PTR && obj -> ptr.pointee) obj = obj -> ptr.pointee;
        if (!obj || obj -> kind != TY_STRUCT) {
            lc_error_at(sc -> source_path, n -> line, 0, "field access on non-struct value");
            sc -> had_error = true;
            return make_type(sc -> arena, TY_VOID);
        }
        return sem_struct_field_type(sc, obj -> named.name, n -> field.field, n -> line);
    }
    case ND_INDEX: {
        Type * arr = sem_infer_expr(sc, n -> idx.array);
        if (!arr || arr -> kind != TY_ARRAY) {
            lc_error_at(sc -> source_path, n -> line, 0, "indexing non-array value");
            sc -> had_error = true;
            return make_type(sc -> arena, TY_VOID);
        }
        return arr -> array.elem;
    }
    case ND_UNARY:
        if (n -> unary.op == TOK_STAR) {
            Type * p = sem_infer_expr(sc, n -> unary.operand);
            if (!p || p -> kind != TY_PTR || !p -> ptr.pointee) {
                lc_error_at(sc -> source_path, n -> line, 0, "dereference of non-pointer value");
                sc -> had_error = true;
                return make_type(sc -> arena, TY_VOID);
            }
            return p -> ptr.pointee;
        }
        break;
    default:
        break;
    }
    lc_error_at(sc -> source_path, n -> line, 0, "left-hand side is not assignable");
    sc -> had_error = true;
    return make_type(sc -> arena, TY_VOID);
}

static Type * sem_infer_expr(SemCtx * sc, ASTNode * n) {
    if (!n) return make_type(sc -> arena, TY_VOID);
    const char * saved_source_path = sc -> source_path;
    if (n -> source_path) diag_use_source_path(n -> source_path);
    if (n -> ty && n -> kind != ND_CALL) return n -> ty;

    Type * ty = NULL;
    switch (n -> kind) {
    case ND_INT_LIT:
        ty = make_type(sc -> arena, TY_INT_GENERIC);
        break;
    case ND_FLOAT_LIT:
        ty = make_type(sc -> arena, TY_FLOAT_GENERIC);
        break;
    case ND_BOOL_LIT:
        ty = make_type(sc -> arena, TY_BOOL);
        break;
    case ND_CHAR_LIT:
        ty = make_type(sc -> arena, TY_CHAR);
        break;
    case ND_STRING_LIT:
    case ND_FORMAT_STR:
        ty = make_type(sc -> arena, TY_STRING);
        break;
    case ND_HASH:
        ty = make_type(sc -> arena, TY_CONTEXT);
        break;
    case ND_IDENT: {
        SemEntry * e = sem_lookup(sc, n -> ident.name);
        if (!e) {
            lc_error_at(sc -> source_path, n -> line, 0, "undefined variable '%s'", n -> ident.name);
            sc -> had_error = true;
            ty = make_type(sc -> arena, TY_VOID);
        } else {
            ty = e -> type;
        }
        break;
    }
    case ND_CAST:
        ty = n -> cast.target ? n -> cast.target : make_type(sc -> arena, TY_VOID);
        (void) sem_infer_expr(sc, n -> cast.operand);
        break;
    case ND_UNARY: {
        Type * ot = sem_infer_expr(sc, n -> unary.operand);
        if (n -> unary.op == TOK_BANG) {
            ty = make_type(sc -> arena, TY_BOOL);
            break;
        }
        if (n -> unary.op == TOK_AMP) {
            ty = make_type(sc -> arena, TY_PTR);
            ty -> ptr.pointee = ot;
            break;
        }
        if (n -> unary.op == TOK_STAR) {
            if (!ot || ot -> kind != TY_PTR || !ot -> ptr.pointee) {
                lc_error_at(sc -> source_path, n -> line, 0, "dereference of non-pointer value");
                sc -> had_error = true;
                ty = make_type(sc -> arena, TY_VOID);
            } else {
                ty = ot -> ptr.pointee;
            }
            break;
        }
        if (!ot || !(sem_is_num_kind(ot -> kind) || ot -> kind == TY_INT_GENERIC || ot -> kind == TY_FLOAT_GENERIC)) {
            lc_error_at(sc -> source_path, n -> line, 0, "unary operator requires a numeric value");
            sc -> had_error = true;
            ty = make_type(sc -> arena, TY_VOID);
        } else {
            ty = ot;
        }
        break;
    }
    case ND_BINARY: {
        Type * lt = sem_infer_expr(sc, n -> binary.lhs);
        Type * rt = sem_infer_expr(sc, n -> binary.rhs);
        switch (n -> binary.op) {
        case TOK_PLUS:
            if ((lt && lt -> kind == TY_STRING) || (rt && rt -> kind == TY_STRING)) {
                if (!(lt && lt -> kind == TY_STRING && rt && rt -> kind == TY_STRING)) {
                    lc_error_at(sc -> source_path, n -> line, 0, "string concatenation requires two strings");
                    sc -> had_error = true;
                }
                ty = make_type(sc -> arena, TY_STRING);
            } else if (lt && rt && sem_is_num_kind(lt -> kind) && sem_is_num_kind(rt -> kind)) {
                ty = sem_is_float_kind(lt -> kind) || sem_is_float_kind(rt -> kind) ?
                    make_type(sc -> arena, TY_FLOAT_GENERIC) :
                    make_type(sc -> arena, TY_INT_GENERIC);
            } else {
                lc_error_at(sc -> source_path, n -> line, 0, "operator '+' requires numeric values or strings");
                sc -> had_error = true;
                ty = make_type(sc -> arena, TY_VOID);
            }
            break;
        case TOK_MINUS:
        case TOK_STAR:
        case TOK_SLASH:
        case TOK_PERCENT:
        case TOK_AMP:
        case TOK_PIPE:
        case TOK_CARET:
        case TOK_LSHIFT:
        case TOK_RSHIFT:
            if (!(lt && rt && sem_is_num_kind(lt -> kind) && sem_is_num_kind(rt -> kind))) {
                lc_error_at(sc -> source_path, n -> line, 0, "operator requires numeric values");
                sc -> had_error = true;
                ty = make_type(sc -> arena, TY_VOID);
            } else {
                ty = sem_is_float_kind(lt -> kind) || sem_is_float_kind(rt -> kind) ?
                    make_type(sc -> arena, TY_FLOAT_GENERIC) :
                    make_type(sc -> arena, TY_INT_GENERIC);
            }
            break;
        case TOK_EQEQ:
        case TOK_NEQ:
        case TOK_LANGLE:
        case TOK_RANGLE:
        case TOK_LTE:
        case TOK_GTE:
        case TOK_ANDAND:
        case TOK_OROR:
            if (!sem_types_compatible(lt, rt) && !sem_types_compatible(rt, lt)) {
                lc_error_at(sc -> source_path, n -> line, 0, "comparison requires compatible operands");
                sc -> had_error = true;
            }
            ty = make_type(sc -> arena, TY_BOOL);
            break;
        default:
            ty = make_type(sc -> arena, TY_VOID);
            break;
        }
        break;
    }
    case ND_FIELD: {
        Type * ot = sem_infer_expr(sc, n -> field.object);
        if (ot && ot -> kind == TY_PTR && ot -> ptr.pointee) ot = ot -> ptr.pointee;
        if (!ot || ot -> kind != TY_STRUCT) {
            lc_error_at(sc -> source_path, n -> line, 0, "field access requires a struct value");
            sc -> had_error = true;
            ty = make_type(sc -> arena, TY_VOID);
        } else {
            ty = sem_struct_field_type(sc, ot -> named.name, n -> field.field, n -> line);
        }
        break;
    }
    case ND_INDEX: {
        Type * at = sem_infer_expr(sc, n -> idx.array);
        (void) sem_infer_expr(sc, n -> idx.index);
        if (!at || at -> kind != TY_ARRAY) {
            lc_error_at(sc -> source_path, n -> line, 0, "indexing requires an array");
            sc -> had_error = true;
            ty = make_type(sc -> arena, TY_VOID);
        } else {
            ty = at -> array.elem;
        }
        break;
    }
    case ND_ENUM_FIELD: {
        int vidx = -1;
        ty = make_type(sc -> arena, TY_ENUM);
        ty -> named.name = n -> enum_field.enum_name;
        Type * payload = sem_enum_variant_type(sc, n -> enum_field.enum_name, n -> enum_field.variant, &
            vidx, n -> line);
        if (payload && n -> enum_field.arg_count > 0) {
            if (n -> enum_field.arg_count != 1) {
                lc_error_at(sc -> source_path, n -> line, 0,
                    "enum variant '%s->%s' takes at most one payload",
                    n -> enum_field.enum_name, n -> enum_field.variant);
                sc -> had_error = true;
            } else {
                Type * at = sem_infer_expr(sc, n -> enum_field.args[0]);
                if (!sem_types_compatible(at, payload)) {
                    lc_error_at(sc -> source_path, n -> line, 0, "enum variant payload has incompatible type");
                    sc -> had_error = true;
                }
            }
        } else if (payload && n -> enum_field.arg_count == 0 && payload) {
            lc_error_at(sc -> source_path, n -> line, 0,
                "enum variant '%s->%s' requires a payload",
                n -> enum_field.enum_name, n -> enum_field.variant);
            sc -> had_error = true;
        }
        break;
    }
    case ND_STRUCT_INIT: {
        ASTNode * sd = sem_find_struct(sc -> program, n -> struct_init.struct_name);
        if (!sd) {
            lc_error_at(sc -> source_path, n -> line, 0, "unknown struct '%s'", n -> struct_init.struct_name);
            sc -> had_error = true;
            ty = make_type(sc -> arena, TY_VOID);
            break;
        }
        ty = make_type(sc -> arena, TY_STRUCT);
        ty -> named.name = sd -> strct.name;
        for (int i = 0; i < n -> struct_init.field_count; i++) {
            const char * fname = n -> struct_init.field_names[i];
            int fidx = -1;
            for (int j = 0; j < sd -> strct.field_count; j++) {
                if (strcmp(sd -> strct.field_names[j], fname) == 0) {
                    fidx = j;
                    break;
                }
            }
            if (fidx < 0) {
                lc_error_at(sc -> source_path, n -> line, 0, "struct '%s' has no field '%s'", sd -> strct.name, fname);
                sc -> had_error = true;
                continue;
            }
            Type * at = sem_infer_expr(sc, n -> struct_init.field_vals[i]);
            if (!sem_types_compatible(at, sd -> strct.field_types[fidx])) {
                lc_error_at(sc -> source_path, n -> line, 0, "field '%s' has incompatible type", fname);
                sc -> had_error = true;
            }
        }
        break;
    }
    case ND_TERNARY: {
        Type * ct = sem_infer_expr(sc, n -> ternary.cond);
        if (!ct || (!sem_is_num_kind(ct -> kind) && ct -> kind != TY_BOOL && ct -> kind != TY_INT_GENERIC)) {
            lc_error_at(sc -> source_path, n -> line, 0, "ternary condition must be boolean or numeric");
            sc -> had_error = true;
        }
        Type * tt = sem_infer_expr(sc, n -> ternary.then_val);
        Type * et = sem_infer_expr(sc, n -> ternary.else_val);
        if (!sem_types_compatible(tt, et) && !sem_types_compatible(et, tt)) {
            lc_error_at(sc -> source_path, n -> line, 0, "ternary branches must have compatible types");
            sc -> had_error = true;
        }
        ty = tt;
        break;
    }
    case ND_CALL: {
        if (n -> call.callee -> kind != ND_IDENT) {
            (void) sem_infer_expr(sc, n -> call.callee);
            for (int i = 0; i < n -> call.arg_count; i++)
                (void) sem_infer_expr(sc, n -> call.args[i]);
            ty = NULL;
            break;
        }
        ASTNode * fd = sem_find_func(sc -> program, n -> call.callee -> ident.name);
        if (!fd) {
            lc_error_at(sc -> source_path, n -> line, 0, "call to undefined function '%s'", n -> call.callee -> ident.name);
            sc -> had_error = true;
            ty = make_type(sc -> arena, TY_VOID);
            break;
        }
        for (int i = 0; i < n -> call.arg_count; i++)
            (void) sem_infer_expr(sc, n -> call.args[i]);
        ty = fd -> func.ret_type ? fd -> func.ret_type : make_type(sc -> arena, TY_VOID);
        break;
    }

    case ND_ASSIGN: {
        Type * lt = sem_infer_lvalue(sc, n -> assign.lhs);
        Type * rt = sem_infer_expr(sc, n -> assign.rhs);
        if (!sem_types_compatible(rt, lt)) {
            lc_error_at(sc -> source_path, n -> line, 0, "cannot assign %s to %s",
                type_to_llvm(rt), type_to_llvm(lt));
            sc -> had_error = true;
        }
        ty = lt;
        break;
    }
    case ND_EXPR_STMT:
        ty = sem_infer_expr(sc, n -> expr_stmt.expr);
        break;
    default:
        ty = make_type(sc -> arena, TY_VOID);
        break;
    }

    n -> ty = ty;
    sc -> source_path = saved_source_path;
    return ty;
}

static void sem_check_stmt(SemCtx * sc, ASTNode * n) {
    if (!n) return;
    const char * saved_source_path = sc -> source_path;
    if (n -> source_path) diag_use_source_path(n -> source_path);
    switch (n -> kind) {
    case ND_BLOCK:
        sem_push(sc);
        for (int i = 0; i < n -> block.count; i++) sem_check_stmt(sc, n -> block.stmts[i]);
        sem_pop(sc);
        break;
    case ND_VAR_DECL: {
        Type * ty = n ->
            var.type;
        if (n ->
            var.init && n ->
            var.init -> kind == ND_CALL && ty)
            n ->
            var.init -> ty = ty;
        if (!ty && n ->
            var.init) ty = sem_infer_expr(sc, n ->
            var.init);
        if (!ty) ty = make_type(sc -> arena, TY_VOID);
        if (ty -> kind == TY_STRUCT && !sem_type_known(sc, ty, sc -> cur_generic)) {
            sem_report_unknown_type(sc, ty -> named.name, n -> line);
        } else if (ty -> kind == TY_ENUM && !sem_type_known(sc, ty, sc -> cur_generic)) {
            sem_report_unknown_type(sc, ty -> named.name, n -> line);
        } else if (!sem_type_known(sc, ty, sc -> cur_generic)) {
            lc_error_at(sc -> source_path, n -> line, source_first_content_col_at(sc -> source_path, n -> line), "unknown type");
            sc -> had_error = true;
        }
        sem_define(sc, n ->
            var.name, ty, n -> line);
        if (n ->
            var.init) {
            Type * it = sem_infer_expr(sc, n ->
                var.init);
            if (!sem_types_compatible(it, ty)) {
                lc_error_at(sc -> source_path, n -> line, 0, "cannot initialize '%s' with %s", n ->
                    var.name, type_to_llvm(it));
                sc -> had_error = true;
            }
        }
        break;
    }
    case ND_ASSIGN: {
        Type * lt = sem_infer_lvalue(sc, n -> assign.lhs);
        if (n -> assign.rhs && n -> assign.rhs -> kind == ND_CALL && lt &&
            (!n -> assign.rhs -> ty || n -> assign.rhs -> ty -> kind == TY_CONTEXT))
            n -> assign.rhs -> ty = lt;
        (void) sem_infer_expr(sc, n);
        break;
    }
    case ND_RETURN: {
        Type * expected = sc -> cur_ret ? sc -> cur_ret : make_type(sc -> arena, TY_VOID);
        if (n -> ret.val && n -> ret.val -> kind == ND_CALL)
            n -> ret.val -> ty = expected;
        if (n -> ret.val) {
            Type * rt = sem_infer_expr(sc, n -> ret.val);
            if (expected -> kind == TY_VOID) {
                lc_error_at(sc -> source_path, n -> line, 0, "void function cannot return a value of type %s",
                    type_to_llvm(rt));
                sc -> had_error = true;
            } else if (!sem_types_compatible(rt, expected)) {
                lc_error_at(sc -> source_path, n -> line, 0, "return type %s does not match function return type %s",
                    type_to_llvm(rt), type_to_llvm(expected));
                sc -> had_error = true;
            }
        }
        break;
    }
    case ND_IF:
        (void) sem_infer_expr(sc, n -> ifstmt.cond);
        sem_check_stmt(sc, n -> ifstmt.then_body);
        if (n -> ifstmt.else_body) sem_check_stmt(sc, n -> ifstmt.else_body);
        break;
    case ND_LOOP:
        sem_push(sc);
        if (n -> loop.counter) sem_check_stmt(sc, n -> loop.counter);
        if (n -> loop.cond)(void) sem_infer_expr(sc, n -> loop.cond);
        if (n -> loop.step)(void) sem_infer_expr(sc, n -> loop.step);
        sem_check_stmt(sc, n -> loop.body);
        sem_pop(sc);
        break;
    case ND_MATCH:
        (void) sem_infer_expr(sc, n -> match.subject);
        for (int i = 0; i < n -> match.arm_count; i++) {
            sem_push(sc);
            ASTNode * arm = n -> match.arms[i];
            if (arm -> match_arm.bind_name) {
                Type * bt = make_type(sc -> arena, TY_VOID);
                if (n -> match.subject && n -> match.subject -> ty && n -> match.subject -> ty -> kind == TY_ENUM) {
                    ASTNode * ed = sem_find_enum(sc -> program, n -> match.subject -> ty -> named.name);
                    if (ed) {
                        for (int j = 0; j < ed -> enm.variant_count; j++) {
                            if (strcmp(ed -> enm.variant_names[j], arm -> match_arm.variant) == 0) {
                                if (ed -> enm.variant_types[j]) bt = ed -> enm.variant_types[j];
                                break;
                            }
                        }
                    }
                }
                sem_define(sc, arm -> match_arm.bind_name, bt, arm -> line);
            }
            sem_check_stmt(sc, arm -> match_arm.body);
            sem_pop(sc);
        }
        break;
    case ND_TYPESWITCH:
        for (int i = 0; i < n -> typeswitch.case_count; i++)
            sem_check_stmt(sc, n -> typeswitch.cases[i] -> typecase.body);
        if (n -> typeswitch.fallback)
            sem_check_stmt(sc, n -> typeswitch.fallback);
        break;
    case ND_CRUMBLE:
        break;
    case ND_EXPR_STMT:
        (void) sem_infer_expr(sc, n -> expr_stmt.expr);
        break;
    default:
        break;
    }
    sc -> source_path = saved_source_path;
}

void semantic_check(ASTNode * program) {
    SemCtx sc = {
        0
    };
    sc.program = program;
    sc.arena = g_arena;
    sem_push( & sc);

    for (int i = 0; i < program -> program.count; i++) {
        ASTNode * it = program -> program.items[i];
        diag_use_source_path(it -> source_path);
        switch (it -> kind) {
        case ND_GLOBAL_VAR: {
            Type * ty = it ->
                var.type ? it ->
                var.type : make_type(sc.arena, TY_I64);
            if (ty -> kind == TY_STRUCT && !sem_type_known( & sc, ty, NULL)) {
                sem_report_unknown_type( & sc, ty -> named.name, it -> line);
            } else if (ty -> kind == TY_ENUM && !sem_type_known( & sc, ty, NULL)) {
                sem_report_unknown_type( & sc, ty -> named.name, it -> line);
            } else if (!sem_type_known( & sc, ty, NULL)) {
                lc_error_at(sc.source_path, it -> line, source_first_content_col_at(sc.source_path, it -> line), "unknown type");
                sc.had_error = true;
            }
            sem_define( & sc, it ->
                var.name, ty, it -> line);
            break;
        }
        case ND_STRUCT_DECL: {
            Type * ty = make_type(sc.arena, TY_STRUCT);
            ty -> named.name = it -> strct.name;
            sem_define( & sc, it -> strct.name, ty, it -> line);
            break;
        }
        case ND_ENUM_DECL: {
            Type * ty = make_type(sc.arena, TY_ENUM);
            ty -> named.name = it -> enm.name;
            sem_define( & sc, it -> enm.name, ty, it -> line);
            break;
        }
        case ND_FUNC_DECL:
        case ND_EXTERN_DECL: {
            if (it -> func.generic_param) {
                for (int k = 0; k < it -> func.param_count; k++)
                    it -> func.param_types[k] = sem_normalize_generic_type(it -> func.param_types[k], it -> func.generic_param);
                it -> func.ret_type = sem_normalize_generic_type(it -> func.ret_type, it -> func.generic_param);
            }
            bool is_dup = false;
            for (int j = 0; j < i; j++) {
                ASTNode * prev = program -> program.items[j];
                if ((prev -> kind == ND_FUNC_DECL || prev -> kind == ND_EXTERN_DECL) &&
                    strcmp(prev -> func.name, it -> func.name) == 0) {
                    lc_error_tok_at(sc.source_path, it -> line, source_first_content_col_at(sc.source_path, it -> line),
                        (int) strlen(it -> func.name),
                        "duplicate function definition '%s' (first defined at line %d)",
                        it -> func.name, prev -> line);
                    is_dup = true;
                    break;
                }
            }
            if (!is_dup) {
                Type * fty = make_type(sc.arena, TY_FUNC);
                fty -> func.params = it -> func.param_types;
                fty -> func.param_count = it -> func.param_count;
                fty -> func.ret = it -> func.ret_type ? it -> func.ret_type : make_type(sc.arena, TY_VOID);
                fty -> func.variadic = it -> func.variadic;
                sem_define( & sc, it -> func.name, fty, it -> line);
            }
            break;
        }
        default:
            break;
        }
    }

    for (int i = 0; i < program -> program.count; i++) {
        ASTNode * it = program -> program.items[i];
        diag_use_source_path(it -> source_path);
        switch (it -> kind) {
        case ND_GLOBAL_VAR:
            if (it ->
                var.init) {
                if (it ->
                    var.init -> kind == ND_CALL && it ->
                    var.type)
                    it ->
                    var.init -> ty = it ->
                    var.type;
                Type * it_ty = sem_infer_expr( & sc, it ->
                    var.init);
                Type * decl = it ->
                    var.type ? it ->
                    var.type : it_ty;
                if (!sem_types_compatible(it_ty, decl)) {
                    lc_error_at(sc.source_path, it -> line, 0, "cannot initialize global '%s' with %s", it ->
                        var.name, type_to_llvm(it_ty));
                    sc.had_error = true;
                }
                if (!it ->
                    var.type) it ->
                    var.type = decl;
            }
            break;
        case ND_FUNC_DECL:
            sem_push( & sc);
            sc.cur_generic = it -> func.generic_param;
            for (int j = 0; j < it -> func.param_count; j++) {
                if (!sem_type_known( & sc, it -> func.param_types[j], sc.cur_generic)) {
                    sem_report_unknown_type( & sc, it -> func.param_types[j] && it -> func.param_types[j] -> kind == TY_STRUCT ? it -> func.param_types[j] -> named.name : "type", it -> line);
                }
                sem_define( & sc, it -> func.param_names[j], it -> func.param_types[j], it -> line);
            }
            if (it -> func.ret_type && !sem_type_known( & sc, it -> func.ret_type, sc.cur_generic)) {
                sem_report_unknown_type( & sc, it -> func.ret_type -> kind == TY_STRUCT || it -> func.ret_type -> kind == TY_ENUM ? it -> func.ret_type -> named.name : "type", it -> line);
            }
            sc.cur_ret = it -> func.ret_type ? it -> func.ret_type : make_type(sc.arena, TY_VOID);
            if (it -> func.body) sem_check_stmt( & sc, it -> func.body);
            sc.cur_generic = NULL;
            sem_pop( & sc);
            break;
        case ND_EXTERN_DECL:
        case ND_STRUCT_DECL:
        case ND_ENUM_DECL:
        case ND_NOMAIN:
        case ND_IMPORT:
        case ND_IN_EXT:
            break;
        default:
            break;
        }
    }

}

/* ================================================================
   SYMBOL TABLE
   ================================================================ */

static SymScope * scope_push(Codegen * cg) {
    SymScope * s = arena_alloc(cg -> arena, sizeof(SymScope));
    s -> parent = cg -> scope;
    cg -> scope = s;
    return s;
}

static void scope_pop(Codegen * cg) {
    cg -> scope = cg -> scope -> parent;
}

static void sym_define(Codegen * cg,
    const char * name, Type * ty,
        const char * llvm_name, bool global) {
    SymEntry * e = arena_alloc(cg -> arena, sizeof(SymEntry));
    e -> name = name;
    e -> type = ty;
    e -> llvm_name = llvm_name;
    e -> is_global = global;
    e -> next = cg -> scope -> entries;
    cg -> scope -> entries = e;
}

static SymEntry * sym_lookup(Codegen * cg,
    const char * name) {
    for (SymScope * s = cg -> scope; s; s = s -> parent)
        for (SymEntry * e = s -> entries; e; e = e -> next)
            if (strcmp(e -> name, name) == 0) return e;
    return NULL;
}

/* ================================================================
   HELPERS
   ================================================================ */

static int new_tmp(Codegen * cg) {
    return ++cg -> tmp;
}
static int new_lbl(Codegen * cg) {
    return ++cg -> lbl;
}

static
const char * tmp_name(Codegen * cg, int id) {
    return arena_sprintf(cg -> arena, "%%t%d", id);
}

static
const char * lbl_name(Codegen * cg, int id) {
    return arena_sprintf(cg -> arena, "lbl%d", id);
}

static
const char * intern_string(Codegen * cg,
    const char * s) {

    for (int i = 0; i < cg -> str_count; i++)
        if (strcmp(cg -> str_pool[i], s) == 0)
            return arena_sprintf(cg -> arena, "@.str%d", i);

    if (cg -> str_count >= cg -> str_cap) {
        int nc = cg -> str_cap == 0 ? 16 : cg -> str_cap * 2;
        const char ** np = arena_alloc(cg -> arena, sizeof(char * ) * (size_t) nc);
        if (cg -> str_pool) memcpy(np, cg -> str_pool, sizeof(char * ) * (size_t) cg -> str_count);
        cg -> str_pool = np;
        cg -> str_cap = nc;
    }
    int idx = cg -> str_count++;
    cg -> str_pool[idx] = arena_strdup(cg -> arena, s);
    return arena_sprintf(cg -> arena, "@.str%d", idx);
}

static ASTNode * find_struct(Codegen * cg,
    const char * name) {
    for (int i = 0; i < cg -> struct_count; i++)
        if (strcmp(cg -> struct_decls[i] -> strct.name, name) == 0)
            return cg -> struct_decls[i];
    return NULL;
}

static ASTNode * find_enum(Codegen * cg,
    const char * name) {
    for (int i = 0; i < cg -> enum_count; i++)
        if (strcmp(cg -> enum_decls[i] -> enm.name, name) == 0)
            return cg -> enum_decls[i];
    return NULL;
}

static ASTNode * find_func(Codegen * cg,
    const char * name) {
    for (int i = 0; i < cg -> func_count; i++) {
        ASTNode * f = cg -> func_decls[i];
        if (strcmp(f -> func.name, name) == 0) return f;
    }
    return NULL;
}

static
const char * named_llvm_type(Codegen * cg,
    const char * name) {
    if (find_enum(cg, name)) return arena_sprintf(cg -> arena, "%%enum.%s", name);
    if (find_struct(cg, name)) return arena_sprintf(cg -> arena, "%%struct.%s", name);

    return "i64";
}

static
const char * cg_type_to_llvm(Codegen * cg, Type * t) {
    if (!t) return "void";
    if (t -> kind == TY_STRUCT || t -> kind == TY_ENUM)
        return named_llvm_type(cg, t -> named.name);
    if (t -> kind == TY_GENERIC) {
        if (cg -> generic_name && t -> generic.param && strcmp(cg -> generic_name, t -> generic.param) == 0 && cg -> generic_actual)
            return cg_type_to_llvm(cg, cg -> generic_actual);
        return "i64";
    }
    if (t -> kind == TY_CONTEXT) {
        if (cg -> context_actual)
            return cg_type_to_llvm(cg, cg -> context_actual);
        return "ptr";
    }
    return type_to_llvm(t);
}

static Type * clone_type_subst(Codegen * cg, Type * t,
    const char * gname, Type * actual) {
    if (!t) return NULL;
    if (gname && strcmp(gname, "#") == 0 && t -> kind == TY_CONTEXT)
        return actual;
    if (t -> kind == TY_GENERIC && t -> generic.param && gname && strcmp(t -> generic.param, gname) == 0)
        return actual;
    Type * n = make_type(cg -> arena, t -> kind);
    switch (t -> kind) {
    case TY_ARRAY:
        n -> array.count = t -> array.count;
        n -> array.elem = clone_type_subst(cg, t -> array.elem, gname, actual);
        break;
    case TY_PTR:
        n -> ptr.pointee = clone_type_subst(cg, t -> ptr.pointee, gname, actual);
        break;
    case TY_STRUCT:
    case TY_ENUM:
        n -> named.name = t -> named.name;
        break;
    case TY_FUNC:
        n -> func.param_count = t -> func.param_count;
        n -> func.variadic = t -> func.variadic;
        if (t -> func.param_count > 0) {
            n -> func.params = arena_alloc(cg -> arena, sizeof(Type * ) * (size_t) t -> func.param_count);
            for (int i = 0; i < t -> func.param_count; i++)
                n -> func.params[i] = clone_type_subst(cg, t -> func.params[i], gname, actual);
        }
        n -> func.ret = clone_type_subst(cg, t -> func.ret, gname, actual);
        break;
    case TY_GENERIC:
        n -> generic.param = t -> generic.param;
        break;
    default:
        *
        n = * t;
        break;
    }
    if (t -> kind == TY_STRUCT || t -> kind == TY_ENUM) n -> named.name = t -> named.name;
    if (t -> kind == TY_GENERIC) n -> generic.param = t -> generic.param;
    return n;
}

static void mangle_type_into(char * buf, size_t cap, Type * t) {
    const char * raw = type_to_llvm(t);
    size_t i = 0;
    for (const char * p = raw;* p && i + 1 < cap; p++) {
        unsigned char c = (unsigned char) * p;
        buf[i++] = (isalnum(c) || c == '_') ? (char) c : '_';
    }
    buf[i] = '\0';
}

static
const char * generic_spec_key(Codegen * cg, ASTNode * fn, Type * actual) {
    char mbuf[256];
    mangle_type_into(mbuf, sizeof(mbuf), actual);
    return arena_sprintf(cg -> arena, "%s__%s", fn -> func.name, mbuf);
}

static Type * find_generic_actual_from_call(Codegen * cg, ASTNode * fd, ASTNode * call) {
    (void) cg;
    if (!fd || !fd -> func.generic_param || !call) return NULL;
    const char * gname = fd -> func.generic_param;
    Type * actual = NULL;
    for (int i = 0; i < fd -> func.param_count && i < call -> call.arg_count; i++) {
        Type * formal = fd -> func.param_types[i];
        if (!formal) continue;
        bool hits_generic = false;
        if (formal -> kind == TY_GENERIC && formal -> generic.param && strcmp(formal -> generic.param, gname) == 0)
            hits_generic = true;
        else if (formal -> kind == TY_PTR && formal -> ptr.pointee && formal -> ptr.pointee -> kind == TY_GENERIC &&
            formal -> ptr.pointee -> generic.param && strcmp(formal -> ptr.pointee -> generic.param, gname) == 0)
            hits_generic = true;
        else if (formal -> kind == TY_ARRAY && formal -> array.elem && formal -> array.elem -> kind == TY_GENERIC &&
            formal -> array.elem -> generic.param && strcmp(formal -> array.elem -> generic.param, gname) == 0)
            hits_generic = true;
        if (hits_generic) {
            Type * argt = call -> call.args[i] ? call -> call.args[i] -> ty : NULL;
            if (argt) {
                if (argt -> kind == TY_INT_GENERIC) argt = resolve_int_type();
                else if (argt -> kind == TY_FLOAT_GENERIC) argt = resolve_float_type();
                if (!actual) actual = argt;
                else if (!types_equal(actual, argt)) {
                    lc_error(call -> line, 0, "generic function '%s' needs one consistent concrete type for '%s'", fd -> func.name, gname);
                    g_errors++;
                    return NULL;
                }
            }
        }
    }
    return actual;
}

static
const char * specialize_generic_func(Codegen * cg, ASTNode * fn, Type * actual) {
    if (!fn || !fn -> func.generic_param || !actual) return fn ? fn -> func.name : "";
    const char * key = generic_spec_key(cg, fn, actual);
    for (GenericSpec * sp = cg -> specs; sp; sp = sp -> next) {
        if (sp -> fn == fn && strcmp(sp -> key, key) == 0)
            return sp -> name;
    }

    GenericSpec * sp = arena_alloc(cg -> arena, sizeof(GenericSpec));
    sp -> key = key;
    sp -> generic_name = fn -> func.generic_param;
    sp -> actual = actual;
    sp -> fn = fn;
    sp -> name = arena_sprintf(cg -> arena, "__Lucile__%s_%s", fn -> func.name, key + strlen(fn -> func.name) + 2);
    sp -> emitted = false;
    sp -> spec = arena_alloc(cg -> arena, sizeof(ASTNode));
    * sp -> spec = * fn;
    sp -> spec -> func.generic_param = NULL;
    sp -> spec -> func.name = sp -> name;
    sp -> spec -> func.param_types = arena_alloc(cg -> arena, sizeof(Type * ) * (size_t) fn -> func.param_count);
    for (int i = 0; i < fn -> func.param_count; i++)
        sp -> spec -> func.param_types[i] = clone_type_subst(cg, fn -> func.param_types[i], fn -> func.generic_param, actual);
    sp -> spec -> func.ret_type = clone_type_subst(cg, fn -> func.ret_type, fn -> func.generic_param, actual);
    sp -> next = cg -> specs;
    cg -> specs = sp;
    return sp -> name;
}

static
const char * specialize_context_func(Codegen * cg, ASTNode * fn, Type * actual) {
    if (!fn || !actual) return fn ? fn -> func.name : "";
    char mbuf[256];
    mangle_type_into(mbuf, sizeof(mbuf), actual);
    const char * key = arena_sprintf(cg -> arena, "%s__ctx__%s", fn -> func.name, mbuf);
    for (GenericSpec * sp = cg -> specs; sp; sp = sp -> next) {
        if (sp -> fn == fn && strcmp(sp -> key, key) == 0)
            return sp -> name;
    }

    GenericSpec * sp = arena_alloc(cg -> arena, sizeof(GenericSpec));
    sp -> key = key;
    sp -> generic_name = "#";
    sp -> actual = actual;
    sp -> fn = fn;
    sp -> name = arena_sprintf(cg -> arena, "__Lucile__%s_ctx_%s", fn -> func.name, mbuf);
    sp -> emitted = false;
    sp -> spec = arena_alloc(cg -> arena, sizeof(ASTNode));
    * sp -> spec = * fn;
    sp -> spec -> func.name = sp -> name;
    sp -> spec -> func.ret_type = clone_type_subst(cg, fn -> func.ret_type, "#", actual);
    if (sp -> spec -> func.param_count > 0) {
        sp -> spec -> func.param_types = arena_alloc(cg -> arena, sizeof(Type * ) * (size_t) fn -> func.param_count);
        for (int i = 0; i < fn -> func.param_count; i++)
            sp -> spec -> func.param_types[i] = clone_type_subst(cg, fn -> func.param_types[i], "#", actual);
    }
    sp -> next = cg -> specs;
    cg -> specs = sp;
    return sp -> name;
}

#define EMIT(fmt, ...) fprintf(cg -> out, fmt "\n", # #__VA_ARGS__)
#define EMITI(fmt, ...) fprintf(cg -> out, "  "
    fmt "\n", # #__VA_ARGS__)

static void emit_func(Codegen * cg, ASTNode * fn);

static void emit_pending_specializations(Codegen * cg) {
    bool progress;
    do {
        progress = false;
        for (GenericSpec * sp = cg -> specs; sp; sp = sp -> next) {
            if (sp -> emitted || !sp -> spec) continue;
            progress = true;
            sp -> emitted = true;

            const char * save_gname = cg -> generic_name;
            Type * save_gactual = cg -> generic_actual;
            Type * save_ctx = cg -> context_actual;
            if (sp -> generic_name && strcmp(sp -> generic_name, "#") == 0) {
                cg -> generic_name = NULL;
                cg -> generic_actual = NULL;
                cg -> context_actual = sp -> actual;
            } else {
                cg -> generic_name = sp -> generic_name;
                cg -> generic_actual = sp -> actual;
            }
            EMIT("");
            emit_func(cg, sp -> spec);
            cg -> generic_name = save_gname;
            cg -> generic_actual = save_gactual;
            cg -> context_actual = save_ctx;
        }
    } while (progress);
}

/* ================================================================
   EMIT HELPERS
   ================================================================ */

#define EMIT(fmt, ...) fprintf(cg -> out, fmt "\n", # #__VA_ARGS__)
#define EMITI(fmt, ...) fprintf(cg -> out, "  "
    fmt "\n", # #__VA_ARGS__)

static void emit_string_consts(Codegen * cg) {
    for (int i = 0; i < cg -> str_count; i++) {
        const char * s = cg -> str_pool[i];

        int len = (int) strlen(s) + 1;
        fprintf(cg -> out, "@.str%d = private unnamed_addr constant [%d x i8] c\"", i, len);
        for (const char * c = s;* c; c++) {
            if ( * c == '\n') fprintf(cg -> out, "\\0A");
            else if ( * c == '\t') fprintf(cg -> out, "\\09");
            else if ( * c == '\r') fprintf(cg -> out, "\\0D");
            else if ( * c == '"') fprintf(cg -> out, "\\22");
            else if ( * c == '\\') fprintf(cg -> out, "\\5C");
            else if ((unsigned char) * c < 32 || (unsigned char) * c > 126)
                fprintf(cg -> out, "\\%02X", (unsigned char) * c);
            else fprintf(cg -> out, "%c", * c);
        }
        fprintf(cg -> out, "\\00\"\n");
    }
}

static
const char * str_ptr(Codegen * cg,
    const char * s) {
    const char * gname = intern_string(cg, s);
    int len = (int) strlen(s) + 1;
    int t = new_tmp(cg);
    const char * tname = tmp_name(cg, t);

    EMITI("%s = getelementptr inbounds [%d x i8], ptr %s, i64 0, i64 0",
        tname, len, gname);
    return tname;
}

static
const char * zero_init(Type * t) {
    if (!t) return "zeroinitializer";
    switch (t -> kind) {
    case TY_BOOL:
    case TY_I8:
    case TY_U8:
    case TY_I16:
    case TY_U16:
    case TY_I32:
    case TY_U32:
    case TY_I64:
    case TY_U64:
    case TY_INT_GENERIC:
        return "0";
    case TY_F32:
        return "0.0";
    case TY_F64:
    case TY_FLOAT_GENERIC:
        return "0.0";
    case TY_STRING:
        return "null";
    case TY_CHAR:
        return "0";
    default:
        return "zeroinitializer";
    }
}

/* ================================================================
   TYPE INFERENCE FOR EXPRESSIONS
   Returns the LLVM type string for an expression without generating code.
   Used to emit correct types in call arguments.
   ================================================================ */

static Type * resolved_expr_type(Codegen * cg, ASTNode * n) {
    if (!n) return make_type(cg -> arena, TY_I64);

    if (n -> ty) {
        Type * t = n -> ty;
        if (t -> kind == TY_INT_GENERIC) return resolve_int_type();
        if (t -> kind == TY_FLOAT_GENERIC) return resolve_float_type();
        if (t -> kind == TY_CONTEXT) {
            if (cg -> context_actual) return cg -> context_actual;
            if (cg -> cur_ret && cg -> cur_ret -> kind != TY_CONTEXT) return cg -> cur_ret;
        }
        if (t -> kind == TY_GENERIC && cg -> generic_actual)
            return cg -> generic_actual;
        return t;
    }

    switch (n -> kind) {
    case ND_INT_LIT:
        return make_type(cg -> arena, TY_I64);
    case ND_FLOAT_LIT:
        return make_type(cg -> arena, TY_F64);
    case ND_BOOL_LIT:
        return make_type(cg -> arena, TY_BOOL);
    case ND_CHAR_LIT:
        return make_type(cg -> arena, TY_CHAR);
    case ND_STRING_LIT:
    case ND_FORMAT_STR:
        return make_type(cg -> arena, TY_STRING);
    case ND_IDENT: {
        SymEntry * e = sym_lookup(cg, n -> ident.name);
        if (!e || !e -> type) return make_type(cg -> arena, TY_I64);
        if (e -> type -> kind == TY_INT_GENERIC) return resolve_int_type();
        if (e -> type -> kind == TY_FLOAT_GENERIC) return resolve_float_type();
        if (e -> type -> kind == TY_CONTEXT) {
            if (cg -> context_actual) return cg -> context_actual;
            if (cg -> cur_ret && cg -> cur_ret -> kind != TY_CONTEXT) return cg -> cur_ret;
        }
        if (e -> type -> kind == TY_GENERIC && cg -> generic_actual)
            return cg -> generic_actual;
        return e -> type;
    }
    case ND_CAST:
        return n -> cast.target ? n -> cast.target : make_type(cg -> arena, TY_VOID);
    case ND_CALL: {
        if (n -> call.callee -> kind == ND_IDENT) {
            ASTNode * fd = find_func(cg, n -> call.callee -> ident.name);
            if (fd && fd -> func.ret_type) {
                Type * rt = fd -> func.ret_type;
                if (fd -> func.generic_param && cg -> generic_actual)
                    rt = clone_type_subst(cg, rt, fd -> func.generic_param, cg -> generic_actual);
                if (rt && rt -> kind == TY_CONTEXT) {
                    if (cg -> context_actual) return cg -> context_actual;
                    if (cg -> cur_ret && cg -> cur_ret -> kind != TY_CONTEXT) return cg -> cur_ret;
                }
                return rt;
            }
        }
        return make_type(cg -> arena, TY_I64);
    }
    default:
        return make_type(cg -> arena, TY_I64);
    }
}

static
const char * expr_llvm_type(Codegen * cg, ASTNode * n) {
    if (!n) return "i64";
    if (n -> ty && n -> kind != ND_CALL) return type_to_llvm(n -> ty);
    switch (n -> kind) {
    case ND_INT_LIT:
        return "i64";
    case ND_FLOAT_LIT:
        return "double";
    case ND_BOOL_LIT:
        return "i1";
    case ND_CHAR_LIT:
        return "i8";
    case ND_STRING_LIT:
        return "ptr";
    case ND_FORMAT_STR:
        return "ptr";
    case ND_ARRAY_LIT:
        return "ptr";
    case ND_CAST:
        return type_to_llvm(n -> cast.target);
    case ND_BINARY: {

        const char * lt = expr_llvm_type(cg, n -> binary.lhs);
        if (strcmp(lt, "double") == 0 || strcmp(lt, "float") == 0) return lt;
        return expr_llvm_type(cg, n -> binary.rhs);
    }
    case ND_UNARY:
        if (n -> unary.op == TOK_AMP) return "ptr";
        return expr_llvm_type(cg, n -> unary.operand);
    case ND_IDENT: {
        Type * t = resolved_expr_type(cg, n);
        return type_to_llvm(t);
    }
    case ND_CALL: {
        Type * t = resolved_expr_type(cg, n);
        return type_to_llvm(t);
    }
    case ND_INDEX:
        return "i64";
    case ND_FIELD:
        return "i64";
    case ND_TERNARY:
        return expr_llvm_type(cg, n -> ternary.then_val);
    default:
        return "i64";
    }
}

/* ================================================================
   EXPRESSION CODE GENERATION
   Returns the SSA value name (e.g. "%t3") or a literal
   ================================================================ */

static
const char * gen_expr(Codegen * cg, ASTNode * n);
static void gen_stmt(Codegen * cg, ASTNode * n);

static ASTNode * parse_interp_expr(Codegen * cg,
    const char * src, int line) {
    Lexer * lx = lexer_new(cg -> arena, src);
    Parser * p = parser_new(cg -> arena, lx);
    ASTNode * e = parse_expr(p);
    if (p -> had_error) return NULL;
    if (!check(p, TOK_EOF)) {
        lc_error(line, 0, "invalid interpolation expression");
        return NULL;
    }
    return e;
}

static Type * interp_expr_type(Codegen * cg, ASTNode * n) {
    if (!n) return NULL;
    switch (n -> kind) {
    case ND_STRING_LIT:
    case ND_FORMAT_STR:
        return make_type(cg -> arena, TY_STRING);
    case ND_INT_LIT:
        return make_type(cg -> arena, TY_INT_GENERIC);
    case ND_FLOAT_LIT:
        return make_type(cg -> arena, TY_FLOAT_GENERIC);
    case ND_BOOL_LIT:
        return make_type(cg -> arena, TY_BOOL);
    case ND_CHAR_LIT:
        return make_type(cg -> arena, TY_CHAR);
    case ND_IDENT: {
        SymEntry * e = sym_lookup(cg, n -> ident.name);
        if (!e) {
            lc_error(n -> line, 0, "undefined variable '%s'", n -> ident.name);
            return NULL;
        }
        return e -> type;
    }
    case ND_CALL: {
        if (n -> call.callee -> kind != ND_IDENT) {
            (void) interp_expr_type(cg, n -> call.callee);
            for (int i = 0; i < n -> call.arg_count; i++)
                (void) interp_expr_type(cg, n -> call.args[i]);
            return NULL;
        }
        ASTNode * fd = find_func(cg, n -> call.callee -> ident.name);
        if (!fd) {
            lc_error(n -> line, 0, "call to undefined function '%s'", n -> call.callee -> ident.name);
            for (int i = 0; i < n -> call.arg_count; i++)
                (void) interp_expr_type(cg, n -> call.args[i]);
            return NULL;
        }
        for (int i = 0; i < n -> call.arg_count; i++)
            (void) interp_expr_type(cg, n -> call.args[i]);
        return fd -> func.ret_type ? fd -> func.ret_type : make_type(cg -> arena, TY_VOID);
    }
    case ND_CAST:
        (void) interp_expr_type(cg, n -> cast.operand);
        return n -> cast.target;
    case ND_UNARY:
        return interp_expr_type(cg, n -> unary.operand);
    case ND_BINARY: {
        if (n -> binary.op == TOK_EQEQ || n -> binary.op == TOK_NEQ ||
            n -> binary.op == TOK_LANGLE || n -> binary.op == TOK_RANGLE ||
            n -> binary.op == TOK_LTE || n -> binary.op == TOK_GTE ||
            n -> binary.op == TOK_ANDAND || n -> binary.op == TOK_OROR) {
            (void) interp_expr_type(cg, n -> binary.lhs);
            (void) interp_expr_type(cg, n -> binary.rhs);
            return make_type(cg -> arena, TY_BOOL);
        }
        Type * lt = interp_expr_type(cg, n -> binary.lhs);
        Type * rt = interp_expr_type(cg, n -> binary.rhs);
        if ((lt && lt -> kind == TY_STRING) || (rt && rt -> kind == TY_STRING))
            return make_type(cg -> arena, TY_STRING);
        if ((lt && type_is_float(lt)) || (rt && type_is_float(rt)))
            return make_type(cg -> arena, TY_FLOAT_GENERIC);
        return make_type(cg -> arena, TY_INT_GENERIC);
    }
    case ND_TERNARY: {
        Type * tt = interp_expr_type(cg, n -> ternary.then_val);
        Type * et = interp_expr_type(cg, n -> ternary.else_val);
        if (tt && tt -> kind == TY_STRING) return tt;
        if (et && et -> kind == TY_STRING) return et;
        return tt ? tt : et;
    }
    case ND_FIELD: {
        Type * ot = interp_expr_type(cg, n -> field.object);
        if (ot && ot -> kind == TY_PTR && ot -> ptr.pointee) ot = ot -> ptr.pointee;
        if (!ot || ot -> kind != TY_STRUCT) {
            lc_error(n -> line, 0, "field access on non-struct type");
            return NULL;
        }
        ASTNode * sd = find_struct(cg, ot -> named.name);
        if (!sd) {
            lc_error(n -> line, 0, "struct '%s' not found", ot -> named.name);
            return NULL;
        }
        for (int i = 0; i < sd -> strct.field_count; i++) {
            if (strcmp(sd -> strct.field_names[i], n -> field.field) == 0)
                return sd -> strct.field_types[i];
        }
        lc_error(n -> line, 0, "no field '%s' in struct '%s'", n -> field.field, ot -> named.name);
        return NULL;
    }
    case ND_INDEX: {
        Type * at = interp_expr_type(cg, n -> idx.array);
        (void) interp_expr_type(cg, n -> idx.index);
        if (!at || at -> kind != TY_ARRAY) {
            lc_error(n -> line, 0, "indexing requires an array");
            return NULL;
        }
        return at -> array.elem;
    }
    default:
        return NULL;
    }
}

static
const char * gen_format_str(Codegen * cg, ASTNode * n) {
    const char * raw = n -> fmt_str.raw;
    char fmt_final[4096];
    int ffi = 0;
    const char * arg_vals[64];
    const char * arg_tys[64];
    int na = 0;

    for (const char * p = raw;* p;) {
        if ( * p == '{') {
            if (p[1] == '{') {
                fmt_final[ffi++] = '{';
                p += 2;
                continue;
            }

            p++;
            int depth = 1;
            bool in_str = false, in_chr = false, esc = false;
            char expr_buf[2048];
            int ei = 0;

            while ( * p && depth > 0) {
                char c = * p++;
                if (in_str) {
                    if (ei < (int) sizeof(expr_buf) - 1) expr_buf[ei++] = c;
                    if (esc) esc = false;
                    else if (c == '\\') esc = true;
                    else if (c == '"') in_str = false;
                    continue;
                }
                if (in_chr) {
                    if (ei < (int) sizeof(expr_buf) - 1) expr_buf[ei++] = c;
                    if (esc) esc = false;
                    else if (c == '\\') esc = true;
                    else if (c == '\'') in_chr = false;
                    continue;
                }
                if (c == '"') {
                    in_str = true;
                    if (ei < (int) sizeof(expr_buf) - 1) expr_buf[ei++] = c;
                    continue;
                }
                if (c == '\'') {
                    in_chr = true;
                    if (ei < (int) sizeof(expr_buf) - 1) expr_buf[ei++] = c;
                    continue;
                }
                if (c == '{') {
                    depth++;
                    if (ei < (int) sizeof(expr_buf) - 1) expr_buf[ei++] = c;
                    continue;
                }
                if (c == '}') {
                    depth--;
                    if (depth == 0) break;
                    if (ei < (int) sizeof(expr_buf) - 1) expr_buf[ei++] = c;
                    continue;
                }
                if (ei < (int) sizeof(expr_buf) - 1) expr_buf[ei++] = c;
                else {
                    lc_error(n -> line, 0, "interpolation expression too long");
                    return str_ptr(cg, "");
                }
            }
            expr_buf[ei] = '\0';

            char * trim = expr_buf;
            while (isspace((unsigned char) * trim)) trim++;
            char * endtrim = trim + strlen(trim);
            while (endtrim > trim && isspace((unsigned char) endtrim[-1])) * --endtrim = '\0';
            if ( * trim == '\0') {
                lc_error(n -> line, 0, "empty interpolation expression");
                return str_ptr(cg, "");
            }

            ASTNode * expr = parse_interp_expr(cg, trim, n -> line);
            if (!expr) return str_ptr(cg, "");

            Type * et = interp_expr_type(cg, expr);
            if (!et) return str_ptr(cg, "");

            const char * spec = "%s";
            const char * aval = gen_expr(cg, expr);
            const char * aty = cg_type_to_llvm(cg, et);

            if (et -> kind == TY_STRING) {
                spec = "%s";
                aty = "ptr";
            } else if (et -> kind == TY_BOOL) {
                spec = "%d";
                int xt = new_tmp(cg);
                EMITI("%%t%d = zext i1 %s to i32", xt, aval);
                aval = tmp_name(cg, xt);
                aty = "i32";
            } else if (et -> kind == TY_I8 || et -> kind == TY_I16 || et -> kind == TY_U8 || et -> kind == TY_U16 || et -> kind == TY_CHAR) {
                spec = "%d";
                int xt = new_tmp(cg);
                EMITI("%%t%d = zext %s %s to i32", xt, cg_type_to_llvm(cg, et), aval);
                aval = tmp_name(cg, xt);
                aty = "i32";
            } else if (et -> kind == TY_I32 || et -> kind == TY_U32) {
                spec = (et -> kind == TY_U32) ? "%u" : "%d";
                aty = "i32";
            } else if (et -> kind == TY_I64 || et -> kind == TY_U64 || et -> kind == TY_INT_GENERIC) {
                spec = (et -> kind == TY_U64) ? "%lu" : "%ld";
                aty = "i64";
            } else if (et -> kind == TY_F32) {
                spec = "%f";
                int xt = new_tmp(cg);
                EMITI("%%t%d = fpext float %s to double", xt, aval);
                aval = tmp_name(cg, xt);
                aty = "double";
            } else if (et -> kind == TY_F64 || et -> kind == TY_FLOAT_GENERIC) {
                spec = "%f";
                aty = "double";
            } else {
                spec = "%s";
                aty = "ptr";
            }

            arg_vals[na] = aval;
            arg_tys[na] = aty;
            na++;
            for (const char * s = spec;* s; s++) fmt_final[ffi++] = * s;
        } else if ( * p == '}' && p[1] == '}') {
            fmt_final[ffi++] = '}';
            p += 2;
        } else {
            fmt_final[ffi++] = * p++;
        }

        if (ffi >= (int) sizeof(fmt_final) - 1) {
            lc_error(n -> line, 0, "format string too long");
            return str_ptr(cg, "");
        }
    }
    fmt_final[ffi] = '\0';

    if (na == 0) {
        return str_ptr(cg, fmt_final);
    }

    int buf_t = new_tmp(cg);
    EMITI("%%t%d = alloca [8192 x i8]", buf_t);
    int buf_ptr_t = new_tmp(cg);
    EMITI("%%t%d = getelementptr inbounds [8192 x i8], ptr %%t%d, i64 0, i64 0",
        buf_ptr_t, buf_t);

    const char * fmt_ptr = str_ptr(cg, fmt_final);
    fprintf(cg -> out, "  call i32 (ptr, ptr, ...) @sprintf(ptr %%t%d, ptr %s",
        buf_ptr_t, fmt_ptr);
    for (int i = 0; i < na; i++) {
        fprintf(cg -> out, ", %s %s", arg_tys[i], arg_vals[i]);
    }
    fprintf(cg -> out, ")\n");

    return arena_sprintf(cg -> arena, "%%t%d", buf_ptr_t);
}

static
const char * load_var(Codegen * cg, SymEntry * e) {
    if (!e) return "0";
    int t = new_tmp(cg);
    const char * llty = cg_type_to_llvm(cg, e -> type);
    EMITI("%%t%d = load %s, ptr %s", t, llty, e -> llvm_name);
    return tmp_name(cg, t);
}

static
const char * emit_cast(Codegen * cg,
    const char * val,
        Type * src_ty, Type * dst_ty) {
    if (!src_ty || !dst_ty || types_equal(src_ty, dst_ty)) return val;

    const char * src_llvm = type_to_llvm(src_ty);
    const char * dst_llvm = type_to_llvm(dst_ty);
    int t = new_tmp(cg);

    if (type_is_integer(src_ty) && type_is_float(dst_ty)) {
        const char * op = type_is_signed(src_ty) ? "sitofp" : "uitofp";
        EMITI("%%t%d = %s %s %s to %s", t, op, src_llvm, val, dst_llvm);
        return tmp_name(cg, t);
    }
    if (type_is_float(src_ty) && type_is_integer(dst_ty)) {
        const char * op = type_is_signed(dst_ty) ? "fptosi" : "fptoui";
        EMITI("%%t%d = %s %s %s to %s", t, op, src_llvm, val, dst_llvm);
        return tmp_name(cg, t);
    }

    if (type_is_integer(src_ty) && type_is_integer(dst_ty)) {

        int src_bits = 0, dst_bits = 0;
        switch (src_ty -> kind) {
        case TY_BOOL:
            src_bits = 1;
            break;
        case TY_I8:
        case TY_U8:
        case TY_CHAR:
            src_bits = 8;
            break;
        case TY_I16:
        case TY_U16:
            src_bits = 16;
            break;
        case TY_I32:
        case TY_U32:
            src_bits = 32;
            break;
        default:
            src_bits = 64;
            break;
        }
        switch (dst_ty -> kind) {
        case TY_BOOL:
            dst_bits = 1;
            break;
        case TY_I8:
        case TY_U8:
        case TY_CHAR:
            dst_bits = 8;
            break;
        case TY_I16:
        case TY_U16:
            dst_bits = 16;
            break;
        case TY_I32:
        case TY_U32:
            dst_bits = 32;
            break;
        default:
            dst_bits = 64;
            break;
        }
        if (src_bits == dst_bits) return val;
        const char * op = src_bits > dst_bits ? "trunc" :
            (type_is_signed(src_ty) ? "sext" : "zext");
        EMITI("%%t%d = %s %s %s to %s", t, op, src_llvm, val, dst_llvm);
        return tmp_name(cg, t);
    }

    if (type_is_float(src_ty) && type_is_float(dst_ty)) {
        bool src32 = src_ty -> kind == TY_F32;
        bool dst32 = dst_ty -> kind == TY_F32;
        if (src32 == dst32) return val;
        const char * op = src32 ? "fpext" : "fptrunc";
        EMITI("%%t%d = %s %s %s to %s", t, op, src_llvm, val, dst_llvm);
        return tmp_name(cg, t);
    }

    if ((src_ty -> kind == TY_PTR || src_ty -> kind == TY_STRING || src_ty -> kind == TY_GENERIC) && type_is_integer(dst_ty)) {
        EMITI("%%t%d = ptrtoint %s %s to %s", t, src_llvm, val, dst_llvm);
        return tmp_name(cg, t);
    }
    if (type_is_integer(src_ty) && (dst_ty -> kind == TY_PTR || dst_ty -> kind == TY_STRING || dst_ty -> kind == TY_GENERIC)) {
        EMITI("%%t%d = inttoptr %s %s to %s", t, src_llvm, val, dst_llvm);
        return tmp_name(cg, t);
    }
    if (src_ty -> kind == TY_PTR || src_ty -> kind == TY_STRING || dst_ty -> kind == TY_PTR || dst_ty -> kind == TY_STRING) {
        EMITI("%%t%d = bitcast %s %s to %s", t, src_llvm, val, dst_llvm);
        return tmp_name(cg, t);
    }
    return val;
}

static
const char * coerce_value(Codegen * cg,
    const char * val, Type * src_ty, Type * dst_ty) {
    if (!dst_ty) return val;
    if (src_ty && types_equal(src_ty, dst_ty)) return val;
    return emit_cast(cg, val, src_ty, dst_ty);
}

static
const char * gen_expr(Codegen * cg, ASTNode * n) {
    if (!n) return "0";
    Arena * a = cg -> arena;

    switch (n -> kind) {

    case ND_INT_LIT:
        return arena_sprintf(a, "%lld", (long long) n -> int_lit.val);

    case ND_FLOAT_LIT:
        return arena_sprintf(a, "%g", n -> float_lit.val);

    case ND_BOOL_LIT:
        return n -> bool_lit.val ? "1" : "0";

    case ND_CHAR_LIT:
        return arena_sprintf(a, "%d", (unsigned char) n -> char_lit.val);

    case ND_STRING_LIT:
        return str_ptr(cg, n -> str_lit.val);

    case ND_FORMAT_STR:
        return gen_format_str(cg, n);

    case ND_HASH: {
        Type * ctx = cg -> context_actual ? cg -> context_actual : cg -> cur_ret;
        if (!ctx) return "0";
        switch (ctx -> kind) {
        case TY_F32:
        case TY_F64:
        case TY_FLOAT_GENERIC:
            return "0.0";
        case TY_STRING:
        case TY_PTR:
            return "null";
        default:
            return "0";
        }
    }

    case ND_IDENT: {
        SymEntry * e = sym_lookup(cg, n -> ident.name);
        if (!e) {
            lc_error(n -> line, 0, "undefined variable '%s'", n -> ident.name);
            return "0";
        }
        if (e -> is_global) {

            return load_var(cg, e);
        }
        return load_var(cg, e);
    }

    case ND_ARRAY_LIT: {

        Type * elem_ty = NULL;
        if (n -> array_lit.count > 0) {

            elem_ty = make_type(a, TY_I64);
        }
        int sz = n -> array_lit.count;
        int at = new_tmp(cg);
        const char * ety = elem_ty ? type_to_llvm(elem_ty) : "i64";
        EMITI("%%t%d = alloca [%d x %s]", at, sz, ety);
        for (int i = 0; i < sz; i++) {
            const char * val = gen_expr(cg, n -> array_lit.elems[i]);
            int pt = new_tmp(cg);
            EMITI("%%t%d = getelementptr inbounds [%d x %s], ptr %%t%d, i64 0, i64 %d",
                pt, sz, ety, at, i);
            EMITI("store %s %s, ptr %%t%d", ety, val, pt);
        }

        int rt = new_tmp(cg);
        EMITI("%%t%d = getelementptr inbounds [%d x %s], ptr %%t%d, i64 0, i64 0",
            rt, sz, ety, at);
        return tmp_name(cg, rt);
    }

    case ND_UNARY: {
        const char * operand = gen_expr(cg, n -> unary.operand);

        Type * ty = n -> unary.operand -> ty ? n -> unary.operand -> ty : make_type(a, TY_I64);
        const char * llty = type_to_llvm(ty);
        int t = new_tmp(cg);
        switch (n -> unary.op) {
        case TOK_MINUS:
            if (type_is_float(ty))
                EMITI("%%t%d = fneg %s %s", t, llty, operand);
            else
                EMITI("%%t%d = sub %s 0, %s", t, llty, operand);
            break;
        case TOK_BANG:
            EMITI("%%t%d = xor i1 %s, 1", t, operand);
            break;
        case TOK_TILDE:
            EMITI("%%t%d = xor %s %s, -1", t, llty, operand);
            break;
        case TOK_STAR: {

            EMITI("%%t%d = load %s, ptr %s", t, llty, operand);
            break;
        }
        case TOK_AMP: {

            return operand;
        }
        default:
            EMITI("%%t%d = add i64 0, %s", t, operand);
            break;
        }
        return tmp_name(cg, t);
    }

    case ND_BINARY: {
        const char * lval = gen_expr(cg, n -> binary.lhs);
        const char * rval = gen_expr(cg, n -> binary.rhs);

        const char * llty = expr_llvm_type(cg, n -> binary.lhs);

        if (strcmp(llty, "ptr") == 0) {

            int t = new_tmp(cg);
            bool is_cmp = (n -> binary.op == TOK_EQEQ || n -> binary.op == TOK_NEQ);
            if (is_cmp) {
                const char * pred = (n -> binary.op == TOK_EQEQ) ? "eq" : "ne";
                EMITI("%%t%d = icmp %s ptr %s, %s", t, pred, lval, rval);
            } else {
                lc_error_tok(n -> line, 1, 0,
                    "cannot apply operator to string/pointer type");
                EMITI("%%t%d = add i64 0, 0", t);
            }
            return tmp_name(cg, t);
        }

        bool is_fp = (strcmp(llty, "float") == 0 || strcmp(llty, "double") == 0);
        bool is_sig = !(strcmp(llty, "i8") == 0 || strcmp(llty, "i16") == 0 ||
                strcmp(llty, "i32") == 0 || strcmp(llty, "i64") == 0) ?
            false : true;

        Type * ast_ty = n -> binary.lhs -> ty;
        if (ast_ty) is_sig = type_is_signed(ast_ty);

        int t = new_tmp(cg);
        const char * op = NULL;

        switch (n -> binary.op) {
        case TOK_PLUS:
            op = is_fp ? "fadd" : "add";
            break;
        case TOK_MINUS:
            op = is_fp ? "fsub" : "sub";
            break;
        case TOK_STAR:
            op = is_fp ? "fmul" : "mul";
            break;
        case TOK_SLASH:
            op = is_fp ? "fdiv" : (is_sig ? "sdiv" : "udiv");
            break;
        case TOK_PERCENT:
            op = is_fp ? "frem" : (is_sig ? "srem" : "urem");
            break;
        case TOK_AMP:
            op = "and";
            break;
        case TOK_PIPE:
            op = "or";
            break;
        case TOK_CARET:
            op = "xor";
            break;
        case TOK_LSHIFT:
            op = "shl";
            break;
        case TOK_RSHIFT:
            op = is_sig ? "ashr" : "lshr";
            break;

        case TOK_EQEQ:
            EMITI("%%t%d = %s %s %s, %s", t,
                is_fp ? "fcmp oeq" : "icmp eq", llty, lval, rval);
            return tmp_name(cg, t);
        case TOK_NEQ:
            EMITI("%%t%d = %s %s %s, %s", t,
                is_fp ? "fcmp one" : "icmp ne", llty, lval, rval);
            return tmp_name(cg, t);
        case TOK_LANGLE:
            EMITI("%%t%d = %s %s %s, %s", t,
                is_fp ? "fcmp olt" : (is_sig ? "icmp slt" : "icmp ult"),
                llty, lval, rval);
            return tmp_name(cg, t);
        case TOK_RANGLE:
            EMITI("%%t%d = %s %s %s, %s", t,
                is_fp ? "fcmp ogt" : (is_sig ? "icmp sgt" : "icmp ugt"),
                llty, lval, rval);
            return tmp_name(cg, t);
        case TOK_LTE:
            EMITI("%%t%d = %s %s %s, %s", t,
                is_fp ? "fcmp ole" : (is_sig ? "icmp sle" : "icmp ule"),
                llty, lval, rval);
            return tmp_name(cg, t);
        case TOK_GTE:
            EMITI("%%t%d = %s %s %s, %s", t,
                is_fp ? "fcmp oge" : (is_sig ? "icmp sge" : "icmp uge"),
                llty, lval, rval);
            return tmp_name(cg, t);
        case TOK_ANDAND:
            EMITI("%%t%d = and i1 %s, %s", t, lval, rval);
            return tmp_name(cg, t);
        case TOK_OROR:
            EMITI("%%t%d = or i1 %s, %s", t, lval, rval);
            return tmp_name(cg, t);
        default:
            EMITI("%%t%d = add i64 0, 0", t);
            return tmp_name(cg, t);
        }
        EMITI("%%t%d = %s %s %s, %s", t, op, llty, lval, rval);
        return tmp_name(cg, t);
    }

    case ND_CAST: {
        const char * val = gen_expr(cg, n -> cast.operand);

        Type * src = resolved_expr_type(cg, n -> cast.operand);
        if (!src) src = make_type(a, TY_I64);
        return emit_cast(cg, val, src, n -> cast.target);
    }

    case ND_TERNARY: {
        const char * cond = gen_expr(cg, n -> ternary.cond);
        int true_lbl = new_lbl(cg);
        int false_lbl = new_lbl(cg);
        int merge_lbl = new_lbl(cg);

        EMITI("br i1 %s, label %%%s, label %%%s",
            cond, lbl_name(cg, true_lbl), lbl_name(cg, false_lbl));

        Type * ty = n -> ternary.then_val -> ty ? n -> ternary.then_val -> ty : make_type(a, TY_I64);
        const char * llty = type_to_llvm(ty);

        EMIT("%s:", lbl_name(cg, true_lbl));
        const char * tv = gen_expr(cg, n -> ternary.then_val);
        EMITI("br label %%%s", lbl_name(cg, merge_lbl));
        int true_end = cg -> lbl;
        (void) true_end;

        EMIT("%s:", lbl_name(cg, false_lbl));
        const char * fv = gen_expr(cg, n -> ternary.else_val);
        EMITI("br label %%%s", lbl_name(cg, merge_lbl));

        EMIT("%s:", lbl_name(cg, merge_lbl));
        int rt = new_tmp(cg);
        EMITI("%%t%d = phi %s [ %s, %%%s ], [ %s, %%%s ]",
            rt, llty, tv, lbl_name(cg, true_lbl),
            fv, lbl_name(cg, false_lbl));
        return tmp_name(cg, rt);
    }

    case ND_INDEX: {
        const char * arr = gen_expr(cg, n -> idx.array);
        const char * idx = gen_expr(cg, n -> idx.index);

        Type * arr_ty = n -> idx.array -> ty;
        const char * ety = "i64";
        const char * arr_ty_str = "i64";
        if (arr_ty && arr_ty -> kind == TY_ARRAY) {
            ety = type_to_llvm(arr_ty -> array.elem);
            arr_ty_str = type_to_llvm(arr_ty);
        }
        int pt = new_tmp(cg);
        EMITI("%%t%d = getelementptr inbounds %s, ptr %s, i64 0, i64 %s",
            pt, arr_ty_str, arr, idx);
        int rt = new_tmp(cg);
        EMITI("%%t%d = load %s, ptr %%t%d", rt, ety, pt);
        return tmp_name(cg, rt);
    }

    case ND_FIELD: {
        (void) gen_expr(cg, n -> field.object);
        Type * obj_ty = n -> field.object -> ty;

        if (!obj_ty && n -> field.object -> kind == ND_IDENT) {
            SymEntry * e = sym_lookup(cg, n -> field.object -> ident.name);
            if (e) obj_ty = e -> type;
        }

        if (obj_ty && obj_ty -> kind == TY_PTR && obj_ty -> ptr.pointee)
            obj_ty = obj_ty -> ptr.pointee;

        if (!obj_ty || obj_ty -> kind != TY_STRUCT) {

            if (n -> field.object -> kind == ND_IDENT) {
                const char * vname = n -> field.object -> ident.name;
                SymEntry * e = sym_lookup(cg, vname);
                if (e && e -> type && e -> type -> kind == TY_STRUCT) {
                    obj_ty = e -> type;
                } else {
                    lc_error(n -> line, 0, "field access on non-struct type");
                    return "0";
                }
            } else {
                lc_error(n -> line, 0, "field access on non-struct type");
                return "0";
            }
        }
        ASTNode * sd = find_struct(cg, obj_ty -> named.name);
        if (!sd) {
            lc_error(n -> line, 0, "struct '%s' not found", obj_ty -> named.name);
            return "0";
        }
        int fidx = -1;
        for (int i = 0; i < sd -> strct.field_count; i++)
            if (strcmp(sd -> strct.field_names[i], n -> field.field) == 0) {
                fidx = i;
                break;
            }
        if (fidx < 0) {
            lc_error(n -> line, 0, "no field '%s' in struct '%s'", n -> field.field, obj_ty -> named.name);
            return "0";
        }
        Type * fty = sd -> strct.field_types[fidx];
        const char * sllty = type_to_llvm(obj_ty);
        const char * fllty = type_to_llvm(fty);
        int pt = new_tmp(cg);

        const char * obj_ptr = NULL;
        if (n -> field.object -> kind == ND_IDENT) {
            SymEntry * e = sym_lookup(cg, n -> field.object -> ident.name);
            if (e) obj_ptr = e -> llvm_name;
        }
        if (!obj_ptr) {

            lc_error(n -> line, 0, "cannot take address of expression for field access");
            return "0";
        }
        EMITI("%%t%d = getelementptr inbounds %s, ptr %s, i32 0, i32 %d",
            pt, sllty, obj_ptr, fidx);
        int rt = new_tmp(cg);
        EMITI("%%t%d = load %s, ptr %%t%d", rt, fllty, pt);
        return tmp_name(cg, rt);
    }

    case ND_ENUM_FIELD: {

        ASTNode * ed = find_enum(cg, n -> enum_field.enum_name);
        if (!ed) {
            lc_error(n -> line, 0, "enum '%s' not found", n -> enum_field.enum_name);
            return "0";
        }
        int tag = -1;
        for (int i = 0; i < ed -> enm.variant_count; i++)
            if (strcmp(ed -> enm.variant_names[i], n -> enum_field.variant) == 0) {
                tag = i;
                break;
            }
        if (tag < 0) {
            lc_error(n -> line, 0, "no variant '%s' in enum '%s'",
                n -> enum_field.variant, n -> enum_field.enum_name);
            return "0";
        }

        const char * ety = arena_sprintf(a, "%%enum.%s", n -> enum_field.enum_name);
        int et = new_tmp(cg);
        EMITI("%%t%d = alloca %s", et, ety);

        int tp = new_tmp(cg);
        EMITI("%%t%d = getelementptr inbounds %s, ptr %%t%d, i32 0, i32 0",
            tp, ety, et);
        EMITI("store i32 %d, ptr %%t%d", tag, tp);

        if (n -> enum_field.arg_count > 0) {
            const char * pval = gen_expr(cg, n -> enum_field.args[0]);
            Type * pt2 = ed -> enm.variant_types[tag];

            if (!pt2 || pt2 -> kind == TY_GENERIC || pt2 -> kind == TY_CONTEXT)
                pt2 = make_type(a, TY_I64);
            const char * pty_s = cg_type_to_llvm(cg, pt2);
            int pp = new_tmp(cg);

            EMITI("%%t%d = getelementptr inbounds %s, ptr %%t%d, i32 0, i32 1",
                pp, ety, et);

            EMITI("store %s %s, ptr %%t%d", pty_s, pval, pp);
        }

        int rt = new_tmp(cg);
        EMITI("%%t%d = load %s, ptr %%t%d", rt, ety, et);
        return tmp_name(cg, rt);
    }

    case ND_STRUCT_INIT: {
        ASTNode * sd = find_struct(cg, n -> struct_init.struct_name);
        const char * sty = arena_sprintf(a, "%%struct.%s", n -> struct_init.struct_name);
        int st = new_tmp(cg);
        EMITI("%%t%d = alloca %s", st, sty);
        for (int i = 0; i < n -> struct_init.field_count; i++) {
            const char * fname = n -> struct_init.field_names[i];
            int fidx = -1;
            if (sd) {
                for (int j = 0; j < sd -> strct.field_count; j++)
                    if (strcmp(sd -> strct.field_names[j], fname) == 0) {
                        fidx = j;
                        break;
                    }
            }
            if (fidx < 0) {
                lc_error(n -> line, 0, "struct '%s' has no field '%s'", n -> struct_init.struct_name, fname);
                continue;
            }
            const char * fval = gen_expr(cg, n -> struct_init.field_vals[i]);
            Type * fty = (sd && fidx < sd -> strct.field_count) ? sd -> strct.field_types[fidx] : make_type(a, TY_I64);
            const char * fllty = type_to_llvm(fty);
            int fp = new_tmp(cg);
            EMITI("%%t%d = getelementptr inbounds %s, ptr %%t%d, i32 0, i32 %d",
                fp, sty, st, fidx);
            EMITI("store %s %s, ptr %%t%d", fllty, fval, fp);
        }
        int rt = new_tmp(cg);
        EMITI("%%t%d = load %s, ptr %%t%d", rt, sty, st);
        return tmp_name(cg, rt);
    }

    case ND_CALL: {

        const char ** arg_vals = arena_alloc(a, sizeof(char * ) * (size_t)(n -> call.arg_count + 1));
        Type ** arg_tys = arena_alloc(a, sizeof(Type * ) * (size_t)(n -> call.arg_count + 1));
        ASTNode * fd_for_call = NULL;
        const char * fname = NULL;
        Type * ret_ty = NULL;
        bool variadic = false;
        const char * generic_name = NULL;
        Type * generic_actual = NULL;
        Type ** param_tys = NULL;

        if (n -> call.callee -> kind == ND_IDENT)
            fd_for_call = find_func(cg, n -> call.callee -> ident.name);

        if (fd_for_call) {
            ret_ty = fd_for_call -> func.ret_type;
            variadic = fd_for_call -> func.variadic;
            if (fd_for_call -> func.generic_param) {
                generic_name = fd_for_call -> func.generic_param;
                generic_actual = find_generic_actual_from_call(cg, fd_for_call, n);
                if (generic_actual) {
                    param_tys = arena_alloc(a, sizeof(Type * ) * (size_t) fd_for_call -> func.param_count);
                    for (int i = 0; i < fd_for_call -> func.param_count; i++)
                        param_tys[i] = clone_type_subst(cg, fd_for_call -> func.param_types[i], fd_for_call -> func.generic_param, generic_actual);
                }
            } else {
                param_tys = fd_for_call -> func.param_types;
            }
        }

        for (int i = 0; i < n -> call.arg_count; i++) {
            Type * src_ty = resolved_expr_type(cg, n -> call.args[i]);
            Type * dst_ty = (param_tys && fd_for_call && i < fd_for_call -> func.param_count) ?
                param_tys[i] : NULL;
            const char * v = gen_expr(cg, n -> call.args[i]);
            arg_vals[i] = coerce_value(cg, v, src_ty, dst_ty);
            arg_tys[i] = dst_ty ? dst_ty : src_ty;
        }

        if (n -> call.callee -> kind == ND_IDENT) {
            fname = n -> call.callee -> ident.name;
            if (fd_for_call) {
                if (fd_for_call -> func.generic_param) {
                    if (generic_actual) {
                        fname = specialize_generic_func(cg, fd_for_call, generic_actual);
                        ret_ty = clone_type_subst(cg, fd_for_call -> func.ret_type, fd_for_call -> func.generic_param, generic_actual);
                    }
                } else if (ret_ty && ret_ty -> kind == TY_CONTEXT) {
                    Type * actual = (n -> ty && n -> ty -> kind != TY_CONTEXT) ? n -> ty : cg -> context_actual;
                    if (actual) {
                        fname = specialize_context_func(cg, fd_for_call, actual);
                        ret_ty = clone_type_subst(cg, fd_for_call -> func.ret_type, "#", actual);
                    } else {
                        ret_ty = make_type(a, TY_STRING);
                    }
                }
            } else {
                lc_error(n -> line, 0, "call to undefined function '%s'", fname);
                ret_ty = make_type(a, TY_VOID);
                fname = fname ? fname : "missing";
            }
        } else {
            fname = gen_expr(cg, n -> call.callee);
            ret_ty = make_type(a, TY_VOID);
        }

        if (ret_ty && ret_ty -> kind == TY_CONTEXT && n -> ty && n -> ty -> kind != TY_CONTEXT)
            ret_ty = n -> ty;

        const char * save_gname = cg -> generic_name;
        Type * save_gactual = cg -> generic_actual;
        if (generic_name && generic_actual) {
            cg -> generic_name = generic_name;
            cg -> generic_actual = generic_actual;
        }

        const char * ret_llvm = cg_type_to_llvm(cg, ret_ty);
        bool has_ret = ret_ty && ret_ty -> kind != TY_VOID;
        int rt = has_ret ? new_tmp(cg) : -1;

        if (has_ret) fprintf(cg -> out, "  %%t%d = ", rt);
        else fprintf(cg -> out, "  ");

        if (variadic) {
            fprintf(cg -> out, "call %s (", ret_llvm);
            if (fd_for_call) {
                for (int i = 0; i < fd_for_call -> func.param_count; i++) {
                    if (i) fprintf(cg -> out, ", ");
                    fprintf(cg -> out, "%s", cg_type_to_llvm(cg, fd_for_call -> func.param_types[i]));
                }
                if (fd_for_call -> func.variadic) fprintf(cg -> out, ", ...");
            }
            fprintf(cg -> out, ") @%s(", fname);
        } else {
            fprintf(cg -> out, "call %s @%s(", ret_llvm, fname);
        }

        for (int i = 0; i < n -> call.arg_count; i++) {
            if (i) fprintf(cg -> out, ", ");
            const char * aty = arg_tys[i] ? cg_type_to_llvm(cg, arg_tys[i]) :
                expr_llvm_type(cg, n -> call.args[i]);
            fprintf(cg -> out, "%s %s", aty, arg_vals[i]);
        }
        fprintf(cg -> out, ")\n");

        cg -> generic_name = save_gname;
        cg -> generic_actual = save_gactual;

        if (has_ret) return tmp_name(cg, rt);
        return NULL;
    }

    case ND_ASSIGN: {

        Type * lhs_ty = n -> assign.lhs ? n -> assign.lhs -> ty : NULL;
        Type * rhs_ty = n -> assign.rhs ? n -> assign.rhs -> ty : NULL;
        if (!lhs_ty) lhs_ty = make_type(a, TY_I64);
        if (!rhs_ty) rhs_ty = make_type(a, TY_I64);
        if (n -> assign.rhs && n -> assign.rhs -> kind == ND_CALL && lhs_ty &&
            (!n -> assign.rhs -> ty || n -> assign.rhs -> ty -> kind == TY_CONTEXT))
            n -> assign.rhs -> ty = lhs_ty;
        const char * rval = gen_expr(cg, n -> assign.rhs);
        rval = coerce_value(cg, rval, rhs_ty, lhs_ty);
        const char * llty = type_to_llvm(lhs_ty);

        if (n -> assign.op != TOK_EQ) {
            const char * lval = gen_expr(cg, n -> assign.lhs);
            lval = coerce_value(cg, lval, lhs_ty, lhs_ty);
            const char * op = NULL;
            bool fp = type_is_float(lhs_ty);
            bool sg = type_is_signed(lhs_ty);
            int ct = new_tmp(cg);
            switch (n -> assign.op) {
            case TOK_PLUSEQ:
                op = fp ? "fadd" : "add";
                break;
            case TOK_MINUSEQ:
                op = fp ? "fsub" : "sub";
                break;
            case TOK_STAREQ:
                op = fp ? "fmul" : "mul";
                break;
            case TOK_SLASHEQ:
                op = fp ? "fdiv" : (sg ? "sdiv" : "udiv");
                break;
            case TOK_PERCENTEQ:
                op = sg ? "srem" : "urem";
                break;
            case TOK_AMPEQ:
                op = "and";
                break;
            case TOK_PIPEEQ:
                op = "or";
                break;
            case TOK_CARETEQ:
                op = "xor";
                break;
            case TOK_LSHIFTEQ:
                op = "shl";
                break;
            case TOK_RSHIFTEQ:
                op = sg ? "ashr" : "lshr";
                break;
            default:
                op = "add";
                break;
            }
            EMITI("%%t%d = %s %s %s, %s", ct, op, llty, lval, rval);
            rval = tmp_name(cg, ct);
        }

        if (n -> assign.lhs -> kind == ND_IDENT) {
            SymEntry * e = sym_lookup(cg, n -> assign.lhs -> ident.name);
            if (!e) {
                lc_error(n -> line, 0, "undefined variable '%s'", n -> assign.lhs -> ident.name);
                return rval;
            }
            EMITI("store %s %s, ptr %s", llty, rval, e -> llvm_name);
        } else if (n -> assign.lhs -> kind == ND_INDEX) {

            const char * arr = gen_expr(cg, n -> assign.lhs -> idx.array);
            const char * idx = gen_expr(cg, n -> assign.lhs -> idx.index);
            Type * arr_ty = n -> assign.lhs -> idx.array -> ty;
            const char * arr_ty_str = arr_ty ? type_to_llvm(arr_ty) : "[0 x i64]";
            const char * ety2 = arr_ty && arr_ty -> kind == TY_ARRAY ?
                type_to_llvm(arr_ty -> array.elem) : "i64";
            int pt = new_tmp(cg);
            EMITI("%%t%d = getelementptr inbounds %s, ptr %s, i64 0, i64 %s",
                pt, arr_ty_str, arr, idx);
            EMITI("store %s %s, ptr %%t%d", ety2, rval, pt);
        } else if (n -> assign.lhs -> kind == ND_FIELD) {

            ASTNode * obj_node = n -> assign.lhs -> field.object;
            const char * fname2 = n -> assign.lhs -> field.field;
            Type * obj_ty = obj_node -> ty;
            if (!obj_ty || obj_ty -> kind != TY_STRUCT) {
                return rval;
            }
            ASTNode * sd = find_struct(cg, obj_ty -> named.name);
            if (!sd) {
                return rval;
            }
            int fidx = -1;
            for (int i = 0; i < sd -> strct.field_count; i++)
                if (strcmp(sd -> strct.field_names[i], fname2) == 0) {
                    fidx = i;
                    break;
                }
            if (fidx < 0) {
                return rval;
            }
            SymEntry * oe = obj_node -> kind == ND_IDENT ? sym_lookup(cg, obj_node -> ident.name) : NULL;
            if (!oe) {
                return rval;
            }
            const char * sllty = type_to_llvm(obj_ty);
            int fp2 = new_tmp(cg);
            EMITI("%%t%d = getelementptr inbounds %s, ptr %s, i32 0, i32 %d",
                fp2, sllty, oe -> llvm_name, fidx);
            EMITI("store %s %s, ptr %%t%d", llty, rval, fp2);
        } else {

            const char * ptr = gen_expr(cg, n -> assign.lhs);
            EMITI("store %s %s, ptr %s", llty, rval, ptr);
        }
        return rval;
    }

    default:
        lc_error(n -> line, 0, "codegen: unhandled expression kind %d", n -> kind);
        return "0";
    }
}

/* ================================================================
   STATEMENT CODE GENERATION
   ================================================================ */

static void gen_stmt(Codegen * cg, ASTNode * n) {
    if (!n) return;
    Arena * a = cg -> arena;

    switch (n -> kind) {

    case ND_BLOCK:
        scope_push(cg);
        for (int i = 0; i < n -> block.count; i++)
            gen_stmt(cg, n -> block.stmts[i]);
        scope_pop(cg);
        break;

    case ND_VAR_DECL: {
        Type * ty = n ->
            var.type;
        if (!ty) ty = make_type(a, TY_I64);

        if (ty -> kind == TY_INT_GENERIC) ty = resolve_int_type();
        if (ty -> kind == TY_FLOAT_GENERIC) ty = resolve_float_type();

        const char * llty = cg_type_to_llvm(cg, ty);
        int t = new_tmp(cg);
        const char * alloca_name = tmp_name(cg, t);
        EMITI("%s = alloca %s", alloca_name, llty);

        if (n ->
            var.init) {
            if (n ->
                var.init -> kind == ND_CALL && ty && (!n ->
                    var.init -> ty || n ->
                    var.init -> ty -> kind == TY_CONTEXT))
                n ->
                var.init -> ty = ty;
            const char * ival = gen_expr(cg, n ->
                var.init);
            ival = coerce_value(cg, ival, n ->
                var.init ? n ->
                var.init -> ty : NULL, ty);
            EMITI("store %s %s, ptr %s", llty, ival, alloca_name);
        } else {
            EMITI("store %s %s, ptr %s", llty, zero_init(ty), alloca_name);
        }

        sym_define(cg, n ->
            var.name, ty, alloca_name, false);
        break;
    }

    case ND_ASSIGN:
    case ND_EXPR_STMT:
        if (n -> kind == ND_ASSIGN) gen_expr(cg, n);
        else gen_expr(cg, n -> expr_stmt.expr);
        break;

    case ND_RETURN: {
        Type * ret_ty = cg -> cur_ret;
        if (!ret_ty || ret_ty -> kind == TY_VOID) {
            EMITI("ret void");
        } else if (n -> ret.val) {
            const char * val = gen_expr(cg, n -> ret.val);
            val = coerce_value(cg, val, n -> ret.val ? n -> ret.val -> ty : NULL, ret_ty);
            const char * llty = cg_type_to_llvm(cg, ret_ty);
            EMITI("ret %s %s", llty, val);
        } else {
            const char * llty = cg_type_to_llvm(cg, ret_ty);
            EMITI("ret %s %s", llty, zero_init(ret_ty));
        }
        cg -> ret_emitted = true;
        break;
    }

    case ND_IF: {
        const char * cond = gen_expr(cg, n -> ifstmt.cond);
        int then_lbl = new_lbl(cg);
        int else_lbl = new_lbl(cg);
        int merge_lbl = new_lbl(cg);

        EMITI("br i1 %s, label %%%s, label %%%s",
            cond, lbl_name(cg, then_lbl), lbl_name(cg, else_lbl));

        EMIT("%s:", lbl_name(cg, then_lbl));
        gen_stmt(cg, n -> ifstmt.then_body);
        EMITI("br label %%%s", lbl_name(cg, merge_lbl));

        EMIT("%s:", lbl_name(cg, else_lbl));
        if (n -> ifstmt.else_body) gen_stmt(cg, n -> ifstmt.else_body);
        EMITI("br label %%%s", lbl_name(cg, merge_lbl));

        EMIT("%s:", lbl_name(cg, merge_lbl));
        break;
    }

    case ND_LOOP: {
        int header_lbl = new_lbl(cg);
        int body_lbl = new_lbl(cg);
        int exit_lbl = new_lbl(cg);

        scope_push(cg);

        if (n -> loop.counter) gen_stmt(cg, n -> loop.counter);

        EMITI("br label %%%s", lbl_name(cg, header_lbl));
        EMIT("%s:", lbl_name(cg, header_lbl));

        if (n -> loop.cond) {
            const char * cond = gen_expr(cg, n -> loop.cond);
            EMITI("br i1 %s, label %%%s, label %%%s",
                cond, lbl_name(cg, body_lbl), lbl_name(cg, exit_lbl));
        } else {
            EMITI("br label %%%s", lbl_name(cg, body_lbl));
        }

        EMIT("%s:", lbl_name(cg, body_lbl));
        gen_stmt(cg, n -> loop.body);

        if (n -> loop.step) gen_expr(cg, n -> loop.step);

        EMITI("br label %%%s", lbl_name(cg, header_lbl));
        EMIT("%s:", lbl_name(cg, exit_lbl));
        scope_pop(cg);
        break;
    }

    case ND_BREAK:

        EMITI("br label %%loop_exit  ; br");
        break;

    case ND_CONTINUE:
        EMITI("br label %%loop_header  ; cont");
        break;

    case ND_MATCH: {

        ASTNode * subj_node = n -> match.subject;
        const char * enum_name = NULL;
        if (subj_node -> kind == ND_IDENT) {
            SymEntry * e = sym_lookup(cg, subj_node -> ident.name);
            if (e && e -> type && (e -> type -> kind == TY_STRUCT || e -> type -> kind == TY_ENUM))
                enum_name = e -> type -> named.name;
        }
        ASTNode * ed = enum_name ? find_enum(cg, enum_name) : NULL;
        const char * ety = enum_name ?
            arena_sprintf(a, "%%enum.%s", enum_name) :
            "%enum._";

        const char * subj_ptr = NULL;
        if (subj_node -> kind == ND_IDENT) {
            SymEntry * e = sym_lookup(cg, subj_node -> ident.name);
            if (e) subj_ptr = e -> llvm_name;
        }

        int tag_t = new_tmp(cg);
        if (subj_ptr) {
            int tag_ptr = new_tmp(cg);
            EMITI("%%t%d = getelementptr inbounds %s, ptr %s, i32 0, i32 0",
                tag_ptr, ety, subj_ptr);
            EMITI("%%t%d = load i32, ptr %%t%d", tag_t, tag_ptr);
        } else {
            const char * subj = gen_expr(cg, n -> match.subject);
            EMITI("%%t%d = extractvalue %s %s, 0", tag_t, ety, subj);
        }

        int default_lbl = new_lbl(cg);
        int merge_lbl = new_lbl(cg);
        int * arm_lbls = arena_alloc(a, sizeof(int) * (size_t) n -> match.arm_count);
        int * arm_tags = arena_alloc(a, sizeof(int) * (size_t) n -> match.arm_count);
        for (int i = 0; i < n -> match.arm_count; i++) {
            arm_lbls[i] = new_lbl(cg);

            const char * vname = n -> match.arms[i] -> match_arm.variant;
            arm_tags[i] = i;
            if (ed) {
                for (int j = 0; j < ed -> enm.variant_count; j++)
                    if (strcmp(ed -> enm.variant_names[j], vname) == 0) {
                        arm_tags[i] = j;
                        break;
                    }
            }
        }

        fprintf(cg -> out, "  switch i32 %%t%d, label %%%s [\n", tag_t, lbl_name(cg, default_lbl));
        for (int i = 0; i < n -> match.arm_count; i++)
            fprintf(cg -> out, "    i32 %d, label %%%s\n", arm_tags[i], lbl_name(cg, arm_lbls[i]));
        fprintf(cg -> out, "  ]\n");

        for (int i = 0; i < n -> match.arm_count; i++) {
            EMIT("%s:", lbl_name(cg, arm_lbls[i]));
            ASTNode * arm = n -> match.arms[i];
            scope_push(cg);

            if (arm -> match_arm.bind_name && subj_ptr) {

                Type * payload_ty = make_type(a, TY_I64);
                if (ed) {
                    for (int j = 0; j < ed -> enm.variant_count; j++) {
                        if (strcmp(ed -> enm.variant_names[j], arm -> match_arm.variant) == 0) {
                            if (ed -> enm.variant_types[j])
                                payload_ty = ed -> enm.variant_types[j];
                            break;
                        }
                    }
                }

                if (payload_ty -> kind == TY_STRUCT || payload_ty -> kind == TY_GENERIC)
                    payload_ty = make_type(a, TY_I64);
                const char * pty_s = type_to_llvm(payload_ty);

                int pp = new_tmp(cg);
                EMITI("%%t%d = getelementptr inbounds %s, ptr %s, i32 0, i32 1",
                    pp, ety, subj_ptr);
                int lp = new_tmp(cg);
                EMITI("%%t%d = load %s, ptr %%t%d", lp, pty_s, pp);

                int al = new_tmp(cg);
                EMITI("%%t%d = alloca %s", al, pty_s);
                EMITI("store %s %%t%d, ptr %%t%d", pty_s, lp, al);
                sym_define(cg, arm -> match_arm.bind_name, payload_ty, tmp_name(cg, al), false);
            }
            gen_stmt(cg, arm -> match_arm.body);
            scope_pop(cg);
            EMITI("br label %%%s", lbl_name(cg, merge_lbl));
        }
        EMIT("%s:", lbl_name(cg, default_lbl));
        EMITI("br label %%%s", lbl_name(cg, merge_lbl));
        EMIT("%s:", lbl_name(cg, merge_lbl));
        break;
    }

    case ND_TYPESWITCH: {
        Type * subject_ty = NULL;
        const char * subject_name = n -> typeswitch.subject_name;
        if (subject_name && strcmp(subject_name, "#") == 0) {
            subject_ty = cg -> context_actual ? cg -> context_actual : cg -> cur_ret;
        } else if (subject_name) {
            SymEntry * se = sym_lookup(cg, subject_name);
            if (se) subject_ty = se -> type;
        }
        if (subject_ty) {
            if (subject_ty -> kind == TY_INT_GENERIC) subject_ty = resolve_int_type();
            else if (subject_ty -> kind == TY_FLOAT_GENERIC) subject_ty = resolve_float_type();
        }

        int chosen = -1;
        if (subject_ty) {
            for (int i = 0; i < n -> typeswitch.case_count; i++) {
                Type * mt = n -> typeswitch.cases[i] -> typecase.match_type;
                if (mt) {
                    if (mt -> kind == TY_INT_GENERIC) mt = resolve_int_type();
                    else if (mt -> kind == TY_FLOAT_GENERIC) mt = resolve_float_type();
                }
                if (mt && types_equal(mt, subject_ty)) {
                    chosen = i;
                    break;
                }
            }
        }

        if (chosen >= 0) {
            ASTNode * tc = n -> typeswitch.cases[chosen];
            scope_push(cg);
            if (subject_name && strcmp(subject_name, "#") != 0) {
                SymEntry * se = sym_lookup(cg, subject_name);
                if (se) {
                    Type * mt = tc -> typecase.match_type ? tc -> typecase.match_type : se -> type;
                    sym_define(cg, subject_name, mt, se -> llvm_name, se -> is_global);
                }
            }
            gen_stmt(cg, tc -> typecase.body);
            scope_pop(cg);
        } else if (n -> typeswitch.fallback) {
            gen_stmt(cg, n -> typeswitch.fallback);
        } else if (n -> typeswitch.case_count > 0) {
            if (!(subject_ty && (subject_ty -> kind == TY_GENERIC || subject_ty -> kind == TY_CONTEXT)))
                lc_error(n -> line, 0, "typeswitch has no matching case for current type");
        }
        break;
    }

    case ND_CRUMBLE:

        break;

    default:
        lc_error(n -> line, 0, "codegen: unhandled statement kind %d", n -> kind);
        break;
    }
}

/* ================================================================
   TOP-LEVEL CODEGEN
   ================================================================ */

static void emit_struct_types(Codegen * cg) {
    for (int i = 0; i < cg -> struct_count; i++) {
        ASTNode * s = cg -> struct_decls[i];
        if (s -> strct.packed)
            fprintf(cg -> out, "%%struct.%s = type <{ ", s -> strct.name);
        else
            fprintf(cg -> out, "%%struct.%s = type { ", s -> strct.name);
        for (int j = 0; j < s -> strct.field_count; j++) {
            if (j) fprintf(cg -> out, ", ");
            fprintf(cg -> out, "%s", type_to_llvm(s -> strct.field_types[j]));
        }
        fprintf(cg -> out, s -> strct.packed ? " }>\n" : " }\n");
    }
}

static void emit_enum_types(Codegen * cg) {
    for (int i = 0; i < cg -> enum_count; i++) {
        ASTNode * e = cg -> enum_decls[i];

        int max_payload = 8;
        for (int j = 0; j < e -> enm.variant_count; j++) {
            if (e -> enm.variant_types[j]) {

                Type * pt = e -> enm.variant_types[j];
                int sz = 8;
                switch (pt -> kind) {
                case TY_I8:
                case TY_U8:
                case TY_BOOL:
                case TY_CHAR:
                    sz = 1;
                    break;
                case TY_I16:
                case TY_U16:
                    sz = 2;
                    break;
                case TY_I32:
                case TY_U32:
                case TY_F32:
                    sz = 4;
                    break;
                default:
                    sz = 8;
                    break;
                }
                if (sz > max_payload) max_payload = sz;
            }
        }
        fprintf(cg -> out, "%%enum.%s = type { i32, [%d x i8] }\n",
            e -> enm.name, max_payload);
    }
}

static void emit_globals(Codegen * cg) {
    for (int i = 0; i < cg -> program -> program.count; i++) {
        ASTNode * item = cg -> program -> program.items[i];
        if (item -> kind != ND_GLOBAL_VAR) continue;

        Type * ty = item ->
            var.type;
        if (!ty) ty = make_type(cg -> arena, TY_I64);
        const char * llty = type_to_llvm(ty);
        const char * gname = arena_sprintf(cg -> arena, "@%s", item ->
            var.name);

        if (item ->
            var.init && item ->
            var.init -> kind == ND_INT_LIT) {
            fprintf(cg -> out, "%s = global %s %lld\n",
                gname, llty, (long long) item ->
                var.init -> int_lit.val);
        } else if (item ->
            var.init && item ->
            var.init -> kind == ND_BOOL_LIT) {
            fprintf(cg -> out, "%s = global %s %d\n",
                gname, llty, item ->
                var.init -> bool_lit.val ? 1 : 0);
        } else if (item ->
            var.init && item ->
            var.init -> kind == ND_STRING_LIT) {

            fprintf(cg -> out, "%s = global %s null\n", gname, llty);
        } else {
            fprintf(cg -> out, "%s = global %s zeroinitializer\n", gname, llty);
        }

        sym_define(cg, item ->
            var.name, ty, gname, true);
    }
}

static void emit_func(Codegen * cg, ASTNode * fn) {
    Type * ret_ty = fn -> func.ret_type;
    if (!ret_ty) ret_ty = make_type(cg -> arena, TY_VOID);
    const char * ret_llvm = cg_type_to_llvm(cg, ret_ty);
    const char * fname = fn -> func.name;

    fprintf(cg -> out, "define %s @%s(", ret_llvm, fname);
    for (int i = 0; i < fn -> func.param_count; i++) {
        if (i) fprintf(cg -> out, ", ");
        fprintf(cg -> out, "%s %%%s.arg", cg_type_to_llvm(cg, fn -> func.param_types[i]),
            fn -> func.param_names[i]);
    }
    fprintf(cg -> out, ") {\n");
    EMIT("entry:");

    const char * save_generic_name = cg -> generic_name;
    Type * save_generic_actual = cg -> generic_actual;
    Type * save_context_actual = cg -> context_actual;
    cg -> cur_func = fname;
    cg -> cur_ret = ret_ty;
    cg -> ret_emitted = false;

    scope_push(cg);

    for (int i = 0; i < fn -> func.param_count; i++) {
        Type * pty = fn -> func.param_types[i];
        const char * llty = cg_type_to_llvm(cg, pty);
        int t = new_tmp(cg);
        EMITI("%%t%d = alloca %s", t, llty);
        EMITI("store %s %%%s.arg, ptr %%t%d", llty, fn -> func.param_names[i], t);
        sym_define(cg, fn -> func.param_names[i], pty, tmp_name(cg, t), false);
    }

    if (fn -> func.body) {

        for (int i = 0; i < fn -> func.body -> block.count; i++)
            gen_stmt(cg, fn -> func.body -> block.stmts[i]);
    }

    if (!cg -> ret_emitted) {
        if (ret_ty -> kind == TY_VOID)
            EMITI("ret void");
        else
            EMITI("ret %s %s", ret_llvm, zero_init(ret_ty));
    }

    scope_pop(cg);
    cg -> generic_name = save_generic_name;
    cg -> generic_actual = save_generic_actual;
    cg -> context_actual = save_context_actual;
    EMIT("}");
}

static void emit_extern(Codegen * cg, ASTNode * fn) {
    Type * ret_ty = fn -> func.ret_type;
    if (!ret_ty) ret_ty = make_type(cg -> arena, TY_VOID);
    const char * ret_llvm = type_to_llvm(ret_ty);

    fprintf(cg -> out, "declare %s @%s(", ret_llvm, fn -> func.name);
    for (int i = 0; i < fn -> func.param_count; i++) {
        if (i) fprintf(cg -> out, ", ");
        fprintf(cg -> out, "%s", type_to_llvm(fn -> func.param_types[i]));
    }
    if (fn -> func.variadic) {
        if (fn -> func.param_count > 0) fprintf(cg -> out, ", ");
        fprintf(cg -> out, "...");
    }
    fprintf(cg -> out, ")\n");

    Type * fty = make_type(cg -> arena, TY_FUNC);
    fty -> func.ret = ret_ty;
    sym_define(cg, fn -> func.name, fty,
        arena_sprintf(cg -> arena, "@%s", fn -> func.name), true);
}

/* ================================================================
   COLLECT PASS — gather all top-level declarations
   ================================================================ */

static void collect_decls(Codegen * cg) {
    Arena * a = cg -> arena;

    int sc = 0, ec = 0, fc = 0, xc = 0;
    for (int i = 0; i < cg -> program -> program.count; i++) {
        ASTNode * item = cg -> program -> program.items[i];
        switch (item -> kind) {
        case ND_STRUCT_DECL:
            sc++;
            break;
        case ND_ENUM_DECL:
            ec++;
            break;
        case ND_FUNC_DECL:
            fc++;
            break;
        case ND_EXTERN_DECL:
            xc++;
            break;
        default:
            break;
        }
    }

    cg -> struct_decls = arena_alloc(a, sizeof(ASTNode * ) * (size_t)(sc + 1));
    cg -> enum_decls = arena_alloc(a, sizeof(ASTNode * ) * (size_t)(ec + 1));
    cg -> func_decls = arena_alloc(a, sizeof(ASTNode * ) * (size_t)(fc + xc + 1));
    cg -> in_exts = arena_alloc(a, sizeof(ASTNode * ) * 32);

    for (int i = 0; i < cg -> program -> program.count; i++) {
        ASTNode * item = cg -> program -> program.items[i];
        switch (item -> kind) {
        case ND_STRUCT_DECL:
            cg -> struct_decls[cg -> struct_count++] = item;
            break;
        case ND_ENUM_DECL:
            cg -> enum_decls[cg -> enum_count++] = item;
            break;
        case ND_FUNC_DECL:
            cg -> func_decls[cg -> func_count++] = item;
            break;
        case ND_EXTERN_DECL:
            cg -> func_decls[cg -> func_count++] = item;
            break;
        case ND_NOMAIN:
            cg -> nomain = true;
            break;
        case ND_IN_EXT:
            if (cg -> in_ext_count < 31)
                cg -> in_exts[cg -> in_ext_count++] = item;
            break;
        default:
            break;
        }
    }
}

/* ================================================================
   MAIN CODEGEN ENTRY
   ================================================================ */

void codegen_run(FILE * out, ASTNode * program, Arena * a) {
    Codegen _cg = {
        0
    };
    Codegen * cg = & _cg;
    cg -> out = out;
    cg -> arena = a;
    cg -> program = program;
    cg -> target_datalayout = g_target_datalayout;
    cg -> target_triple = g_target_triple;
    cg -> context_actual = NULL;

    cg -> scope = arena_alloc(a, sizeof(SymScope));

    collect_decls(cg);

    EMIT("; Module Lucile");
    if (cg -> target_datalayout) EMIT("target datalayout = \"%s\"", cg -> target_datalayout);
    else EMIT("target datalayout = \"e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128\"");
    if (cg -> target_triple) EMIT("target triple = \"%s\"", cg -> target_triple);
    EMIT("");

    emit_struct_types(cg);
    emit_enum_types(cg);

    EMIT("declare i32 @sprintf(ptr, ptr, ...)");
    for (int i = 0; i < cg -> program -> program.count; i++) {
        ASTNode * item = cg -> program -> program.items[i];
        if (item -> kind == ND_EXTERN_DECL) emit_extern(cg, item);
    }

    emit_globals(cg);

    bool has_main = false;
    for (int i = 0; i < cg -> program -> program.count; i++) {
        ASTNode * item = cg -> program -> program.items[i];
        if (item -> kind != ND_FUNC_DECL) continue;
        if (item -> func.generic_param) continue;
        if (item -> func.ret_type && item -> func.ret_type -> kind == TY_CONTEXT) continue;
        if (strcmp(item -> func.name, "main") == 0) has_main = true;
        EMIT("");
        emit_func(cg, item);
    }

    emit_pending_specializations(cg);

    if (!cg -> nomain && !has_main) {
        lc_error(0, 0, "no 'main' function found (add @nomain; if intentional)");
    }

    EMIT("");
    emit_string_consts(cg);
}
