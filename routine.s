.align 4
.section __TEXT,__text
.global _routine
_routine:
    mov x11, x0
    add x11, x11, #4096
routine_mk_chk:
    ldr x12, [x11, #-16]
    cmp x12, #0
    b.eq routine_mk_end
routine_mk_body:
    add x11, x11, #4096
    b routine_mk_chk
routine_mk_end:
    mov x6, x11
    mov x10, x11
    sub x10, x10, #128
    stp x10, x1, [x11, #-16]!
    sub x11, x11, #80
    stp xzr, x2, [x11, #-16]! 
    mov x0, x6
    ret 

.global _routine_yield
_routine_yield:

    mov x11, x29
    add x11, x11, #144

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

.global _routine_run
_routine_run:
    mov x11, x0
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
    mov x11, x29
    add x11, x11, #144

    stp xzr, xzr, [x11, #-16]!
    sub x11, x11, #16
    ldp x29, x30, [x11, #-16]!
    sub x11, x11, #64
    ldr x12, [x11, #-16]! 
    mov sp, x12
    mov w0, #1
    ret

