
.align 4
.section __TEXT,__text

.global __setjmp
__setjmp:
    adrp x9, _jmp_ptr@PAGE
    add x9, x9, _jmp_ptr@PAGEOFF
    
    adrp x10, _jmp_ptr_offset@PAGE
    add x10, x10, _jmp_ptr_offset@PAGEOFF
    ldr x10, [x10]
    add x9, x9, x10

    str x30, [x9, #-8]!
    str x29, [x9, #-8]!
    mov x10, sp
    str x10, [x9, #-8]!
    str x7, [x9, #-8]!
    str x6, [x9, #-8]!
    str x5, [x9, #-8]!
    str x4, [x9, #-8]!
    str x3, [x9, #-8]!
    str x2, [x9, #-8]!
    str x1, [x9, #-8]!
    str x0, [x9, #-8]!

    adrp x10, _jmp_ptr_offset@PAGE
    add x10, x10, _jmp_ptr_offset@PAGEOFF
    ldr x11, [x10]
    sub x11, x11, #96
    str x11, [x10]

    ; always return 0
    mov w0, #0
    ret
    

.global __longjmp
__longjmp:
    adrp x9, _jmp_ptr@PAGE
    add x9, x9, _jmp_ptr@PAGEOFF

    adrp x10, _jmp_ptr_offset@PAGE
    add x10, x10, _jmp_ptr_offset@PAGEOFF

    ldr x11, [x10]
    add x11, x11, #96
    str x11, [x10]

    cmp x11, #4096
    b.gt longjmp_ret

    add x9, x9, x11

    ldr x30, [x9, #-8]!
    ldr x29, [x9, #-8]!
    ldr x10, [x9, #-8]!
    mov sp, x10
    ldr x7, [x9, #-8]!
    ldr x6, [x9, #-8]!
    ldr x5, [x9, #-8]!
    ldr x4, [x9, #-8]!
    ldr x3, [x9, #-8]!
    ldr x2, [x9, #-8]!
    ldr x1, [x9, #-8]!
    ldr x0, [x9, #-8]!
longjmp_ret:
    mov w0, #1
    ret
    
.align 4
.section __DATA,__data
_jmp_ptr_offset: .dword 4096

.align 4
.section __DATA,__bss
_jmp_ptr: .skip 4096

