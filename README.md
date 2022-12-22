# 68000 Assembler

## Summary

An assembler for a range of Motorola 68000 processors, though at present only the original 68000, the '08 and '10 are covered.

The surrent startus is eary, very early, testing.

## Compiling

There are two programs here; the assembler and a tool required to facilitate compiling the assembler:

### asm68k.c

This is the source code for the assembler.  There are no other hand written elements.

### op68k.c

This is a tool which converts a data file of 68000 opcode definitions into data structures which can be included in the "asm68k" assembler source.

This program is called simply (once compiled as "op68k") thus:

```
op68k < ops68k.ops > ops68k.h
```

### ops68k.ops

This file contains the detailed descriptions of the instructions and how they are built into machine code.


Jeff.
