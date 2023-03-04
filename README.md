# LDE
Length Disassembler Engine
==========================

Based on https://github.com/CasualX/lde

Given an address, extract the lengths of the opcodes in it.

Supports `x86` and `x86_64`.

Building
--------

```bash
mkdir lde-build
cd lde-build
cmake ../lde-master
cmake . --build
```

Static library liblde or lde.lib should get built.

Building and running tests
--------------------------

```bash
cd lde-build
cmake . --build --target test
test
```

Usage
-----

```c
#include "lde/lde.h"

...
// x86_32 mode
LdeInstructionLength l32 = lde_get_instruction_length_x86("\x0F\x1F\x40\x00", 4);
assert(l32.total_len == 4);

// x86_64 mode
LdeInstructionLength l64 = lde_get_instruction_length_x86_64("\x66\x66\x0f\x1f\x84\x00\x00\x00\x00\x00", 10);
assert(l64.total_len == 10);

```