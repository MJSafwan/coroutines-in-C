.align 4
.section __TEXT,__text
.global _routine
_routine:
    adrp x2, _nr@PAGE
    add x2, x2, _nr@PAGEOFF
    ldr x3, [x2]
    add x3, x3, #1
    str x3, [x2]

    adrp x4, _nar@PAGE
    add x4, x4, _nar@PAGEOFF

    adrp x6, _rs@PAGE
    add x6, x6, _rs@PAGEOFF

    mov x11, x6
    add x11, x11, #4096
routine_mk_chk:
    ldr x12, [x11, #-16]
    cmp x12, #0
    b.eq routine_mk_end
routine_mk_body:
    add x11, x11, #4096
    b routine_mk_chk
routine_mk_end:
    mov x5, #8
    sub x3, x3, #1
    mul x5, x5, x3
    str x11, [x4, x5]
    mov x10, x11
    sub x10, x10, #128
    stp x10, x0, [x11, #-16]!
    sub x11, x11, #80
    stp xzr, x1, [x11, #-16]! 
    ret 

.global _routine_yield
_routine_yield:
    adrp x4, _narp@PAGE
    add x4, x4, _narp@PAGEOFF
    ldr x5, [x4]
    sub x5, x5, #1

    adrp x10, _nar@PAGE
    add x10, x10, _nar@PAGEOFF

    mov x6, #8
    mul x6, x6, x5
    ldr x11, [x10, x6]

    mov x12, sp
    stp x12, x30, [x11, #-16]!
    stp x29, xzr, [x11, #-16]!
    ldp x29, x30, [x11, #-16]!
    stp x7, x6, [x11, #-16]!
    stp x5, x4, [x11, #-16]!
    stp x3, x2, [x11, #-16]!
    stp x1, x0, [x11, #-16]!

    ldr x12, [x11, #-16]! 
    mov sp, x12
    mov w0, #0
    ret

.global _routine_next
_routine_next:
    adrp x0, _nr@PAGE
    add x0, x0, _nr@PAGEOFF
    ldr x1, [x0]
    ;sub x1, x1, #1

    mov w0, #2
    cmp x1, #0
    b.eq routine_next_finish
    mov w0, #0
    
    adrp x2, _narp@PAGE
    add x2, x2, _narp@PAGEOFF
    ldr x3, [x2]

    cmp x3, x1
    b.ge routine_reduce
    b routine_continue
routine_reduce:
    mov x3, #0
routine_continue:
    adrp x4, _nar@PAGE
    add x4, x4, _nar@PAGEOFF

    mov x5, #8
    mul x5, x5, x3
    ldr x11, [x4, x5]
    
    add x3, x3, #1
    str x3, [x2]

    ldp x13, x10, [x11, #-16]!
    mov x12, x10
    cmp x12, #0
    b.eq routine_next_finish
    
    mov x14, x29
    ldp x29, xzr, [x11, #-16]!
    stp x14, x30, [x11, #-16]!
    ldp x7, x6, [x11, #-16]!
    ldp x5, x4, [x11, #-16]!
    ldp x3, x2, [x11, #-16]!
    ldp x1, x0, [x11, #-16]!

    mov x12, sp
    str x12, [x11, #-16]! 
    mov sp, x13
    br x10
routine_next_finish:
    ret

.global _routine_finish
_routine_finish:
    adrp x2, _nr@PAGE
    add x2, x2, _nr@PAGEOFF
    ldr x3, [x2]

    sub x3, x3, #1
    str x3, [x2]

    adrp x4, _narp@PAGE
    add x4, x4, _narp@PAGEOFF
    ldr x5, [x4]
    sub x5, x5, #1
    str x5, [x4]

    adrp x10, _nar@PAGE
    add x10, x10, _nar@PAGEOFF
rf_chk:
    cmp x5, x3
    b.ge rf_end
rf_body:
    mov x6, #8
    mul x6, x6, x5

    ldr x7, [x10, x6]
    add x6, x6, #8
    ldr x11, [x10, x6]
    str x7, [x10, x6]
    sub x6, x6, #8
    str x11, [x10, x6]

    add x5, x5, #1
    b rf_chk
rf_end:
    ;add x5, x5, #1
    ;str x5, [x4]
    mov x6, #8
    mul x6, x6, x5
    ldr x11, [x10, x6]
    str xzr, [x10, x6]
    
    ;mov x11, x29
    ;add x11, x11, #144
    mov x12, sp
    stp xzr, xzr, [x11, #-16]!
    sub x11, x11, #16
    ldp x29, x30, [x11, #-16]!
    sub x11, x11, #64

    ldr x12, [x11, #-16]! 
    mov sp, x12
    mov w0, #1
    ret

.align 4
.section __DATA,__bss
.global _rs
_rs: .skip 4096*256
_nr: .skip 8
_nar: .skip 8*256
_narp: .skip 8
