.intel_syntax noprefix

.section .text
.global my_memcpy

my_memcpy:
    shl rdx, 32
    shr rdx, 32
    xor r8, r8
    copy_by_eight:
        cmp rdx, 8
        js copy_by_one
        mov r9, [rsi + r8]
        mov [rdi + r8], r9
        add r8, 8
        sub rdx, 8
        jmp copy_by_eight

    copy_by_one:
        cmp rdx, 1
        js done
        mov al, [rsi + r8]
        mov [rdi + r8], al
        add r8, 1
        sub rdx, 1
        jmp copy_by_one

    done:
        mov rax, rdi
        ret
