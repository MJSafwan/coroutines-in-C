.align 4
.section __TEXT,__text
.global _routine
; uint64_t routine(void *ctx, void *fn, void *arg)
_routine:
    adrp x10, _ROUTINE_STACK_SIZE@PAGE
    add x10, x10, _ROUTINE_STACK_SIZE@PAGEOFF
    ldr x10, [x10]
    mov x11, x0
    add x11, x11, x10
routine_mk_chk:
    ldr x12, [x11, #-16]
    cmp x12, #0
    b.eq routine_mk_end
routine_mk_body:
    add x11, x11, x10
    b routine_mk_chk
routine_mk_end:
    mov x6, x11
    mov x10, x11
    sub x10, x10, #240
    stp x10, x1, [x11, #-16]!
    sub x11, x11, #112
    stp x2, xzr, [x11, #-16]! 
    mov x0, x6
    ret 

.global _routine_yield
; void routine_yield(void)
_routine_yield:
    mov x11, x29
    add x11, x11, #256

    mov x12, sp
    ; shamelessly stolen from musl's setjmp implementation (with modification)
    stp x12, x30, [x11, #-16]!
    str x29, [x11, #-8]!
    sub x11, x11, #8
    ldp x29, x30, [x11, #-16]!
    stp x19, x20, [x11, #-16]!
    stp x21, x22, [x11, #-16]!
    stp x23, x24, [x11, #-16]!
    stp x25, x26, [x11, #-16]!
    stp x27, x28, [x11, #-16]!
    stp x0, x1, [x11, #-16]!
    stp d8, d9, [x11, #-16]!
    stp d10, d11, [x11, #-16]!
    stp d12, d13, [x11, #-16]!
    stp d14, d15, [x11, #-16]!
    ldr x12, [x11, #-16]! 

    mov sp, x12
    mov w0, #0
    ret

.global _routine_sleep
; void routine_sleep(uint64_t ms)
_routine_sleep:
    mov x11, x29
    add x11, x11, #256

    mov x12, sp
    stp x12, x30, [x11, #-16]!
    str x29, [x11, #-8]!
    ldr x2, [x11, #-8]!
    ldp x29, x30, [x11, #-16]!
    stp x19, x20, [x11, #-16]!
    stp x21, x22, [x11, #-16]!
    stp x23, x24, [x11, #-16]!
    stp x25, x26, [x11, #-16]!
    stp x27, x28, [x11, #-16]!
    stp x0, x1, [x11, #-16]!
    stp d8, d9, [x11, #-16]!
    stp d10, d11, [x11, #-16]!
    stp d12, d13, [x11, #-16]!
    stp d14, d15, [x11, #-16]!
    ldr x12, [x11, #-16]! 

    mov sp, x12
    cmp x2, 0
    b.eq routine_sleep_ret
    str x0, [x2]
routine_sleep_ret:
    mov w0, #2
    ret


.global _routine_panic
; void routine_panic(uint64_t panic_code)
_routine_panic:
    mov x11, x29
    add x11, x11, #256
    sub x11, x11, #24
    ldr x2, [x11, #-8]!
    ldp x29, x30, [x11, #-16]!
    sub x11, x11, #160
    ldr x12, [x11, #-16]! 
    mov sp, x12
    cmp x2, 0
    b.eq routine_panic_ret
    str x0, [x2]
routine_panic_ret:
    mov w0, #4
    ret

.global _routine_await
; void routine_await(routine_t a_routine)
_routine_await:
    mov x11, x29
    add x11, x11, #256

    mov x12, sp
    stp x12, x30, [x11, #-16]!
    str x29, [x11, #-8]!
    ldr x2, [x11, #-8]!
    ldp x29, x30, [x11, #-16]!
    stp x19, x20, [x11, #-16]!
    stp x21, x22, [x11, #-16]!
    stp x23, x24, [x11, #-16]!
    stp x25, x26, [x11, #-16]!
    stp x27, x28, [x11, #-16]!
    stp x0, x1, [x11, #-16]!
    stp d8, d9, [x11, #-16]!
    stp d10, d11, [x11, #-16]!
    stp d12, d13, [x11, #-16]!
    stp d14, d15, [x11, #-16]!
    ldr x12, [x11, #-16]! 

    mov sp, x12
    cmp x2, 0
    b.eq routine_await_ret
    str x0, [x2]
routine_await_ret:
    mov w0, #3
    ret

.global _routine_run
; int routine_run(uint64_t routine, routine_res *rr)
_routine_run:
    mov x11, x0
    ldp x13, x10, [x11, #-16]!
    cmp x10, #0
    b.eq routine_next_finish
    
    mov x14, x29
    ldr x29, [x11, #-8]!
    str x1, [x11, #-8]!
    stp x14, x30, [x11, #-16]!
    ldp x19, x20, [x11, #-16]!
    ldp x21, x22, [x11, #-16]!
    ldp x23, x24, [x11, #-16]!
    ldp x25, x26, [x11, #-16]!
    ldp x27, x28, [x11, #-16]!
    ldp x0, x1, [x11, #-16]!
    ldp d8, d9, [x11, #-16]!
    ldp d10, d11, [x11, #-16]!
    ldp d12, d13, [x11, #-16]!
    ldp d14, d15, [x11, #-16]!

    mov x12, sp
    str x12, [x11, #-16]! 
    mov sp, x13
    br x10
routine_next_finish:
    ret

.global _routine_finish
; void routine_finish(void)
_routine_finish:
    mov x11, x29
    add x11, x11, #256
    stp xzr, xzr, [x11, #-16]!
    sub x11, x11, #16
    ldp x29, x30, [x11, #-16]!
    sub x11, x11, #160
    ldr x12, [x11, #-16]! 
    mov sp, x12
    mov w0, #1
    ret

.section __DATA,__data
.global _ROUTINE_STACK_SIZE
; uint32_t ROUTINE_STACK_SIZE = 4096
_ROUTINE_STACK_SIZE: .word 4096
