  .text
  .global longest_inc_subseq

// (int64_t* array, uint64_t* help_array, uint64_t size)
// x0, x1, x2
longest_inc_subseq:
  eor x9, x9, x9
  eor x10, x10, x10
  mov x11, 8
  mul x2, x2, x11
  // this is i
  eor x3, x3, x3
  catch_zero_size:
    cmp x2, 0
    bne big_cycle_of_i
    mov x0, 0
    ret
  big_cycle_of_i:
    // // mov [x1, x3], 1
    mov x10, 1
    str x10, [x1, x3]
    // this is j
    eor x4, x4, x4
    small_cycle_of_j:
      ldr x9, [x0, x4] // x9 = array[j]
      ldr x10, [x0, x3] // x10 = array[i]
      // // cmp [x0, x4], [x0, x3]
      cmp x9, x10
      bge skip
      // if d[j] + 1 <= d[i] то скип
      ldr x9, [x1, x4]
      ldr x10, [x1, x3]
      add x9, x9, 1

      cmp x9, x10
      // sub [x1, x4], [x1, x4], 1
      ble skip
      str x9, [x1, x3]
      // mov [x1, x3], [x1, x4]
      // add [x1, x3], [x1, x3], 1
    skip:
      add x4, x4, 8
      cmp x4, x3
      blt small_cycle_of_j
    add x3, x3, 8
    cmp x3, x2
    blt big_cycle_of_i
  // this is ans
  eor x5, x5, x5
  ldr x5, [x1]
  // this is i here
  eor x6, x6, x6
  find_ans_of_i:
    ldr x9, [x1, x6]
    cmp x5, x9
    bge skip_i
    mov x5, x9
    skip_i:
      add x6, x6, 8
      cmp x6, x2
      blt find_ans_of_i
  mov x0, x5
  ret
