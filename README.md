# URCL Emulator
## URCL Emulator
### URCL Emulator
#### URCL Emulator
##### URCL Emulator
Compile emulator:<br>
`$ g++ urcl-emu.cpp -o u2`<br>
Run emulator:<br>
`$ u2 -r <compiled code>`<br>
URCL Emulator made in C++ and Assembler made in Node.JS
I hope it works fine

Compiler supports:
- DW strings (`DW "Hello World!"`)
- Pragma variables (`@define VARNAME, 0xff`, `@define VARNAME, .label`, `@define VARNAME, 'c'`, `@string VARNAME, "string data here"`)
- URCL ISA
