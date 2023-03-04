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