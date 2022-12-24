# 68000 Assembler

## Summary

An assembler for a range of Motorola 68000 processors, though at present only the original 68000, the '08 and '10 are covered.

The surrent startus is eary, very early, testing.

## Facilities

At the moment, while ostensibly supporting the concept of sections (code, data and BSS), and being prepared to assemble source code without any specific base address being provided, it falls short of generating relocatable results.

If no fixed address are provided (via "ORG") then all three sections are floating, and only once code generation is started are fixed addresses applied to each section.  The sections will be ordered traditionally (code, data then BSS), with the output generated starting at address 0.

Assigning an address to the text segment before assembling any code will enable the start address of the text section to be moved and while the remaining sections remain "un-fixed" they will always follow the code section in their respective locations.

The start address of the code can be set using the "START" directive.

Relocatable intermediate object files are a future objective, just need to identify a suitable (and simple) format then embed that into the output processing.

## Usage

```
$ ./a.out --help
Usage: ./a.out [ {options} ] {filename}
Options:-
        --68000          Target mc68000 CPU
        --68008          Target mc68008 CPU
        --68010          Target mc68010 CPU
        --68020          Target mc68020 CPU
        --68030          Target mc68030 CPU
        --68881          Target mc68881 FPU
        --68882          Target mc68882 FPU
        --hexadecimal    Output text hexadecimal values
        --intel          Output Intel Hex format data
        --motorola       Output Motorola S records
        --listing        Display an assembly listing
        --no-output      Do not output any code
        --stdout         Send output to console
        --symbols        Output Symbol table
        --sections       Output consolidated sections
        --dump-opcodes   Display op-codes table
        --help           Display this help information
        --debug          Enable additonal debugging output
```

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

## Warning

This is neither finished nor tested code.  You have been warned.

Jeff.
