# XMX-Emulator
An Emulator for the 16-bit X-Makina ISA designed by Doctor Larry Hughes at Dalhousie University.

Description from Larry Hughes:

X-Makina is a 16-bit Instruction Set Architecture (ISA) design with the following features:

33 instructions (memory access, arithmetic, logic, and control), all 16-bits in width. Many instructions can operate on a byte, a word, or both.

An additional 32 instructions that can be emulated by using existing instructions, allowing operations such as subroutine return, interrupt return, and stack push and pull.

A common instruction format, containing an opcode and one or two operand addresses (for arithmetic and logical instructions) or a signed 10-bit offset (for jump instructions).

Sixteen 16-bit registers: four special purpose (program counter, stack pointer, program status word, and link register) and four general purpose (for data, addressing, or both).

Four addressing modes (register, register with pre- and post- auto-increment and auto-decrement, register-relative, and immediate).
A 16-bit data/address bus, allowing up to 64 KiB of random-access memory.

Traps, Faults and Devices are also implemented. Devices consist of a clock, an input reader and an output to screen.

![image](https://user-images.githubusercontent.com/72110751/177622834-3673503c-bebf-4c52-a00b-ebfa943949f4.png)
