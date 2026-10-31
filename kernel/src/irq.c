#include "irq.h"

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21

#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

#define PIC_EOI      0x20

static inline void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"(value), "Nd"(port)
    );
}

static inline void io_wait(void)
{
    outb(0x80, 0);
}

void irq_init(void)
{
    /* Begin PIC initialization */
    outb(PIC1_COMMAND, 0x11);
    io_wait();

    outb(PIC2_COMMAND, 0x11);
    io_wait();

    /* Remap master PIC to interrupt 32 */
    outb(PIC1_DATA, 0x20);
    io_wait();

    /* Remap slave PIC to interrupt 40 */
    outb(PIC2_DATA, 0x28);
    io_wait();

    /* Configure master and slave PIC */
    outb(PIC1_DATA, 0x04);
    io_wait();

    outb(PIC2_DATA, 0x02);
    io_wait();

    /* Set 8086 mode */
    outb(PIC1_DATA, 0x01);
    io_wait();

    outb(PIC2_DATA, 0x01);
    io_wait();

    /* Mask all IRQs for now */
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}

void irq_handler(unsigned int vector)
{
    /* Ignore invalid interrupt vectors */
    if (vector < 32 || vector > 47)
        return;

    /* Send EOI to slave PIC when needed */
    if (vector >= 40)
    {
        outb(PIC2_COMMAND, PIC_EOI);
    }

    /* Send EOI to master PIC */
    outb(PIC1_COMMAND, PIC_EOI);
}
