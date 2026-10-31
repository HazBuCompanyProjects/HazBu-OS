BITS 32

section .text

extern irq_handler

%macro IRQ 2
global irq%1

irq%1:
    push dword %2
    jmp irq_common
%endmacro

; IRQ 0 - 15
; Interrupt vectors 32 - 47

IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39

IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

irq_common:
    pusha

    ; Clear direction flag for C code
    cld

    ; Get interrupt vector
    mov eax, [esp + 32]

    ; Call C interrupt handler
    push eax
    call irq_handler
    add esp, 4

    ; Restore CPU registers
    popa

    ; Remove interrupt vector
    add esp, 4

    ; Return from interrupt
    iretd
