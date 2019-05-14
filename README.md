# Welcome to Krakatau
[![Build](https://travis-ci.org/egranata/krakatau.svg?branch=master)](https://travis-ci.org/egranata/krakatau)
[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)

Krakatau is a stack-based virtual machine.

It amins to be minimal, fast, orthogonal and easy to program - albeit with permutations of the ordering of those.

Krakatau is based upon the notion of immutable *values* and a *stack*.

## Values

A value is an immutable box that contains one specific kind of datum.

Values can be of one of the following types:

* number (64-bit);
* boolean;
* string;
* tuple (an indexable list of other values);
* table (a key-value store of other values);
* operation (any Krakatau instruction);
* block (a tuple of operations);
* empty (the null value);
* error (invalid operations push errors);
* type (the type of a value);
* bind (a pair of a value and a callable, e.g. a block or operation).

## The stack

The Krakatau VM currently has a single stack on which values can be pushed and popped.

Executing blocks and mutating the stack is the way to perform a computation in the VM.

## Blocks

Blocks are the equivalent to functions in the Krakatau language. A block is a sequence of operations, which are by default executed in order from first to last. There are control flow operations that allow to restart execution of a block or skip the remainder of a block.

Blocks take arguments off the stack. As a convenience, one can define *slots* as part of a block, which are then automatically popped off the stack, and labeled for each execution of a block.

Blocks can be given names and inserted in the *value store*, or defined at the point of use.

## Value store

Krakatau can load programs from a textual language, as well as from a serialized binary format.

Both formats work by describing a series of named values (e.g. `main` of type `block`) and putting these in a storage separate from the stack: the value store. Instructions exist to add and fetch values from this storage area. By convention, a program begins by looking for a block named `main` in the store and executing it.

For example, this is a valid Krakatau program:

```
value compute block slots $a,$b,$c {
  loadslot $a
  loadslot $b
  add
  loadslot $c
  mul
}

value main block {
  push number 5
  dup
  push number 4
  load compute
  exec
}
```

It is roughly equivalent to the C++ program

```
#include <stdio.h>
#include <inttypes.h>

uint64_t compute(uint64_t a, uint64_t b, uint64_t c) {
  return (a+b)*c;
}

int main() {
  printf("%" PRIu64 "\n", compute(4, 5, 5));
  return 0;
}
```

This is a very simple example, but more interesting programs are possible, and blocks can be nested at arbitrary levels of depth, including recursion.

## How to get started

To build Krakatau, you need cmake, lex, and a C++17 compiler (clang and gcc are supported).

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

## Join the conversation

To get involved with the Krakatau project,

* try out new things and file bugs for any issues you run into;
* contribute documentation;
* join krakatau-discuss@googlegroups.com;
* send pull requests (but see CONTRIBUTING.md file before proceeding).

In all cases, enjoy this project but please know that Krakatau is not an officially supported Google product.
