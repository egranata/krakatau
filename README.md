# Welcome to Krakatau

Krakatau is a stack-based virtual machine.

It amins to be minimal, fast, orthogonal and easy to program - albeit with permutations of the ordering of those.

Krakatau is based upon the notion of immutable *values* and a *stack*.

### Values

All values are immutable. A value can be thought of a box that can hold one specific kind of datum.

Value types include:

* number (64-bit);
* boolean;
* string (UTF-8);
* tuple (an indexable list of other values);
* table (a key-value store of other values);
* operation (any Krakatau instruction);
* block (a tuple of operations);
* empty (the null value);
* error (invalid operations push errors);
* type (the type of a value).

### The stack

The Krakatau VM currently has a single stack on which values can be pushed and popped by means of operations.

Executing blocks and mutating the stack is the way to perform a computation in the VM.
For example, this is a valid Krakatau program:

```
value main block {
  push number 24
  push number 3
  add
  push number 2
  mul
}
```

It is equivalent to the expression `(24+3)*2`

More complex programs can be written that involve multiple *blocks* calling each other, as well as more interesting
transformations of the stack. It is also possible to create new blocks at runtime and place them in a *value store*.

### Value store

Krakatau can load programs from a textual language, as well as from a serialized binary format.

Both formats work by describing a series of named values (e.g. `main` of type `block` above) and putting these
in a storage separate from the stack: the value store. Instructions exist to add and fetch values from this
storage area. By convention, a program begins by looking for a block named `main` in the store and executing it:

```
    auto main_val = ms.value_store().retrieve("main");
    auto main_blk = runtime_ptr_cast<Value_Block>(main_val);
    if (main_blk == nullptr) {
        printf("no main block found\n");
        exit(1);
    }
    Push p(main_val);
    p.execute(ms);
    Exec e;
    e.execute(ms);
```

### How to get started

To build Krakatau, you need cmake, lex, and a C++17 compiler.

```
build$ cmake ..
[...]
-- Build files have been written to: /home/egranata/krakatau/build
$ make -j
[...]
build$ ls
assembler  CMakeCache.txt  cmake_install.cmake  googletest-download  lexer.cpp  libstuff.so  runner
bin        CMakeFiles      googletest-build     googletest-src       lib        Makefile     tests
```

The build process generates a series of targets:

* `assembler`: takes source code in the Krakatau language as input and generates a serialized blob;
* `runner`: takes a serialized blob as input and runs the program described by it;
* `tests`: the (Googletest-based) test suite used to validate changes to the VM.

Once a build is complete, run `tests` to check for any issues:

```
build$ ./tests
[...]
[==========] 299 tests from 63 test suites ran. (12 ms total)
[  PASSED  ] 299 tests.
```

### Join the conversation

To get involved with the Krakatau project,

* try out new things and file bugs for any issues you run into;
* contribute documentation;
* join krakatau-discuss@googlegroups.com;
* send pull requests (but see CONTRIBUTING.md file before proceeding).

In all cases, enjoy this project but please know that Krakatau is not an officially supported Google product.
