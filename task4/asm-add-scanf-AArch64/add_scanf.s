  .text
  .global add_scanf

  .macro push Xn
    sub sp, sp, 16
    str \Xn, [sp]
  .endm

  .macro get_address Xn
    sub sp, sp, 16
    mov \Xn, sp
  .endm

  .macro pop Xn
    ldr \Xn, [sp]
    add sp, sp, 16
  .endm

add_scanf:
  push x30
  ldr x0, =scanf_format_string
  
  get_address x1
  get_address x2

  bl scanf

  pop x1
  pop x2

  add x0, x1, x2

  pop x30
  ret

  .section .rodata

scanf_format_string:
  .string "%lld %lld"
