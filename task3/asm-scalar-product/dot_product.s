    .intel_syntax noprefix

    .text
    .global dot_product

// rdi, rsi, rdx
dot_product:

    xor r8, r8
    vxorps ymm0, ymm0, ymm0
    vxorps ymm1, ymm1, ymm1
    vxorps ymm2, ymm2, ymm2
    vxorps ymm3, ymm3, ymm3

    get_by_8:

        cmp rdi, 8
        js hor_sum

        vmovups ymm0, [rsi + r8]
        vmovups ymm1, [rdx + r8]

        add r8, 32
        
        vmulps ymm2, ymm0, ymm1
        vaddps ymm3, ymm2, ymm3

        sub rdi, 8
        jmp get_by_8
    
    hor_sum:
        vxorps xmm0, xmm0, xmm0
        vxorps xmm1, xmm1, xmm1
        vextractf128 xmm0, ymm3, 0
        vextractf128 xmm1, ymm3, 1

        haddps xmm0, xmm0
        haddps xmm1, xmm1
        haddps xmm0, xmm0
        haddps xmm1, xmm1

        vxorps xmm2, xmm2, xmm2
        addss xmm0, xmm1

    get_by_1:

        vxorps xmm4, xmm4, xmm4
        vxorps xmm5, xmm5, xmm5
        vxorps xmm6, xmm6, xmm6

        cmp rdi, 1
        js done

        movss xmm4, [rsi + r8]
        movss xmm5, [rdx + r8]

        add r8, 4
        
        vmulss xmm6, xmm4, xmm5
        vaddss xmm0, xmm6, xmm0

        sub rdi, 1
        jmp get_by_1

    done:
        ret
