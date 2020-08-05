
.thumb_func
.global _start
_start:
stacktop: .word 0x20004fff       ;@ stack pointer at top of ram
    .word reset     ;@ reset handler 
    .word hang
    .word hang
    .word hang
    .word hang
    .word hang
    .word hang
    .word hang
    .word hang
    .word hang
    .word hang
    .word hang
    .word hang
    .word hang
    .word hang

.thumb_func
reset:
    bl kmain
    b hang
.thumb_func
    hang:   b .

;@-----------------------
.end
;@-----------------------
