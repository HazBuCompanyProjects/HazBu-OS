#include "interrupts.h"

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

struct idt_entry
{
    uint16_t offset_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t flags;
    uint16_t offset_high;
} __attribute__((packed));

struct idt_pointer
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

static struct idt_entry idt[256];

extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);

static void (*isr_table[32])(void) =
{
    isr0, isr1, isr2, isr3,
    isr4, isr5, isr6, isr7,
    isr8, isr9, isr10, isr11,
    isr12, isr13, isr14, isr15,
    isr16, isr17, isr18, isr19,
    isr20, isr21, isr22, isr23,
    isr24, isr25, isr26, isr27,
    isr28, isr29, isr30, isr31
};

static void idt_set_gate(
    uint8_t vector,
    void (*handler)(void)
)
{
    uint32_t address = (uint32_t)handler;

    idt[vector].offset_low = address & 0xFFFF;
    idt[vector].selector = 0x08;
    idt[vector].zero = 0;
    idt[vector].flags = 0x8E;
    idt[vector].offset_high = (address >> 16) & 0xFFFF;
}

void interrupts_init(void)
{
    for (int i = 0; i < 256; i++)
    {
        idt[i].offset_low = 0;
        idt[i].selector = 0;
        idt[i].zero = 0;
        idt[i].flags = 0;
        idt[i].offset_high = 0;
    }

    for (int i = 0; i < 32; i++)
    {
        idt_set_gate(i, isr_table[i]);
    }

    struct idt_pointer pointer;

    pointer.limit = sizeof(idt) - 1;
    pointer.base = (uint32_t)&idt;

    __asm__ volatile ("lidt %0" : : "m"(pointer));
}

__attribute__((noreturn))
void exception_handler(uint32_t vector)
{
    volatile uint16_t *video =
        (volatile uint16_t *)0xB8000;

    const char *message = "HAZBU OS KERNEL EXCEPTION: 0x";
    const char *hex = "0123456789ABCDEF";

    unsigned int position = 0;

    while (*message)
    {
        video[position++] =
            (uint16_t)(*message++) | (0x4F << 8);
    }

    for (int shift = 28; shift >= 0; shift -= 4)
    {
        char digit = hex[(vector >> shift) & 0xF];

        video[position++] =
            (uint16_t)digit | (0x4F << 8);
    }

    __asm__ volatile ("cli");

    while (1)
    {
        __asm__ volatile ("hlt");
    }
}
