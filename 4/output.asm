;compile with:
;nasm -f elf32 output.asm
;gcc -o output output.o -m32
section .text

global main
main:
  mov ecx,msg
  mov edx,msg_len
  call output
  
  mov ecx,buf
  mov edx,buf_len
  call input
  
  mov ecx,buf
  mov edx,eax
  call output

  jmp exit

input:
  mov eax,3
  mov ebx,0
  int 80h
  ret

output:
  mov eax,4
  mov ebx,1
  int 80h
  ret

exit:
  mov eax,1
  mov ebx,2
  int 80h
section .data
msg db `ch's output test program using syscall\ninput something and i will repeat it\n`,0
msg_len equ $-msg

section .bss
buf resb 200
buf_len equ $-buf

