.intel_syntax noprefix
.text
.global add_scanf

add_scanf:
    xor rax, rax
    push rdi
    mov rdi, rsp
    lea rdi, [scanf_format_string]
    lea rsi, [a]
    lea rdx, [b]
    call scanf
    mov rax, a
    add rax, b
    pop rdi
    ret
    .section .bss
a: .quad 0
b: .quad 0
    .section .rodata
    
scanf_format_string:
    .string "%lld %lld"

