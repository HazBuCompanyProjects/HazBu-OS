BITS 32

section .text

global gdt_load

gdt_load:
    lgdt [gdt_descriptor]

    ; Reload code segment
    jmp 0x08:.reload_segments

.reload_segments:
    ; Reload data segments
    mov ax, 0x10

    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ret


section .rodata
align 8

gdt_start:

    ; Null descriptor
    dq 0x0000000000000000

    ; Kernel code segment
    dq 0x00CF9A000000FFFF

    ; Kernel data segment
    dq 0x00CF92000000FFFF

gdt_end:


gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start
