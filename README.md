# DSL Compiler — Built with C, Flex & Bison

A minimal but fully functional compiler for a simple Domain-Specific Language (DSL).
It walks through every classic compiler phase: **Lexing → Parsing → AST → Semantic Analysis → Code Generation**.

---

## Project Structure

```
dsl_compiler/
├── src/
│   ├── lexer.l        # Flex  – lexical analyser (tokeniser)
│   ├── parser.y       # Bison – grammar + AST construction
│   ├── ast.h / ast.c  # AST node types and helpers
│   ├── semantic.h / semantic.c   # Semantic analysis (symbol table)
│   ├── codegen.h / codegen.c     # Three-address intermediate code
│   └── main.c         # Driver – wires all phases together
├── test/
│   ├── test1.dsl           # Basic arithmetic assignments
│   ├── test2.dsl           # Longer expression chain
│   └── test3_error.dsl     # Semantic error example
├── output/
│   ├── test1_expected.txt  # Expected output for test1
│   ├── test2_expected.txt  # Expected output for test2
│   └── test3_expected.txt  # Expected output for test3
├── Makefile
└── README.md
```

---

## The DSL Language

### Supported Syntax

| Feature              | Example                    |
|----------------------|----------------------------|
| Integer literal      | `42`                       |
| Variable assignment  | `x = 5;`                   |
| Addition             | `z = x + y;`               |
| Subtraction          | `z = x - y;`               |
| Multiplication       | `z = x * y;`               |
| Division             | `z = x / y;`               |
| Parenthesised expr   | `z = (x + y) * 2;`         |
| Unary minus          | `z = -x;`                  |
| Single-line comment  | `# this is a comment`      |
| Multiple statements  | any number of assignments  |

### Grammar (BNF)

```
program    ::= stmtlist
stmtlist   ::= stmt | stmt stmtlist
stmt       ::= IDENTIFIER '=' expr ';'
expr       ::= expr '+' expr
             | expr '-' expr
             | expr '*' expr
             | expr '/' expr
             | '-' expr
             | '(' expr ')'
             | NUMBER
             | IDENTIFIER
```

---

## Prerequisites

You need three tools installed:

```bash
# Debian / Ubuntu
sudo apt-get install gcc flex bison

# macOS (with Homebrew)
brew install gcc flex bison
```

Verify versions:

```bash
gcc   --version
flex  --version
bison --version
```

---

## Build

```bash
# From the project root
make all
```

This runs three steps automatically:

1. **Bison** reads `src/parser.y` → generates `build/parser.tab.c` and `build/parser.tab.h`
2. **Flex** reads `src/lexer.l`  → generates `build/lex.yy.c`
3. **GCC** compiles all `.c` files together → produces `build/dslc`

---

## Run

```bash
# Basic usage
./build/dslc <source.dsl>

# Also write the three-address code to a file
./build/dslc <source.dsl> <output.tac>
```

---

## Test

```bash
make test       # runs all three tests
make test1      # test/test1.dsl  → output/test1.tac
make test2      # test/test2.dsl  → output/test2.tac
make test3      # test/test3_error.dsl  (expects semantic error)
```

### test1.dsl

```
x = 5;
y = 10;
z = x + y;
result = z * 2;
```

**Expected three-address output:**

```
begin:
    x = 5
    y = 10
    t0 = x + y
    z = t0
    t1 = z * 2
    result = t1
end:
```

### test2.dsl

```
a = 3;
b = 4;
c = a * a + b * b;
d = c - 1;
e = (a + b) * 2;
f = e / 7;
```

**Expected three-address output:**

```
begin:
    a = 3
    b = 4
    t0 = a * a
    t1 = b * b
    t2 = t0 + t1
    c = t2
    t3 = c - 1
    d = t3
    t4 = a + b
    t5 = t4 * 2
    e = t5
    t6 = e / 7
    f = t6
end:
```

### test3_error.dsl (semantic error)

```
a = 10;
c = a + b;   # ERROR: b is not defined
```

**Expected error:**

```
Semantic error: variable 'b' used before assignment
[Semantic] 1 error(s) found
```

---

## Compiler Phases Explained

### Phase 1 — Lexical Analysis (`lexer.l`)

Flex scans the raw source text and breaks it into tokens:

| Token        | Pattern                        |
|--------------|-------------------------------|
| `NUMBER`     | `[0-9]+`                      |
| `IDENTIFIER` | `[a-zA-Z_][a-zA-Z0-9_]*`     |
| `ASSIGN`     | `=`                           |
| `PLUS`       | `+`                           |
| `MINUS`      | `-`                           |
| `TIMES`      | `*`                           |
| `DIVIDE`     | `/`                           |
| `SEMI`       | `;`                           |
| `LPAREN`     | `(`                           |
| `RPAREN`     | `)`                           |

Whitespace and `#`-comments are silently discarded.

### Phase 2 — Parsing (`parser.y`)

Bison applies the grammar rules to the token stream and calls the AST
constructor functions to build a tree. Operator precedence (`* /` before `+ -`)
is declared with `%left` directives so the grammar stays unambiguous.

### Phase 3 — AST Construction (`ast.c`)

Five node kinds:

| Kind           | Meaning                            |
|----------------|------------------------------------|
| `NODE_NUMBER`  | Integer constant                   |
| `NODE_IDENT`   | Variable reference                 |
| `NODE_ASSIGN`  | `left = right`                     |
| `NODE_BINOP`   | `left op right`                    |
| `NODE_STMTLIST`| Ordered list of statements         |

### Phase 4 — Semantic Analysis (`semantic.c`)

A lightweight pass over the AST with a hash-map symbol table:

* For each **assignment** `x = expr`, the RHS expression is checked *first*,
  then `x` is added to the symbol table.
* For every **identifier** used in an expression, the table is queried.
  If the variable is not found an error is reported.
* Compilation is aborted if any semantic errors are found.

### Phase 5 — Intermediate Code Generation (`codegen.c`)

A simple recursive walk that emits **three-address code (TAC)**:

```
t0 = x + y      ← binary operation into a fresh temporary
z  = t0         ← assignment
```

Temporaries are numbered `t0, t1, t2, …`. This TAC is the standard
input format for further back-end optimisation passes or machine-code
generation in a real compiler.

---

## Clean

```bash
make clean
```

Removes `build/` and any generated `.tac` files in `output/`.

---

## Extending the Compiler

Here are straightforward next steps if you want to take it further:

* **`print` statement** — add a `PRINT` token and grammar rule, emit a
  TAC `print x` instruction.
* **`if` / `while`** — add control-flow nodes to the AST and emit
  conditional jumps in the TAC (e.g., `ifFalse t0 goto L1`).
* **Type system** — add float literals and track types in the symbol
  table; emit type-conversion TAC instructions.
* **Register allocation** — map TAC temporaries to real CPU registers.
* **x86-64 / ARM code generation** — translate TAC instructions to
  assembly, assemble with `nasm` or `as`.
