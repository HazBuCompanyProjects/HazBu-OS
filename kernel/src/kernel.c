/* HazBu OS - First Kernel */

typedef unsigned short uint16_t;

void kernel_main(void)
{
    uint16_t *video_memory = (uint16_t *)0xB8000;

    const char *message = "Welcome to HazBu OS!";

    for (int i = 0; i < 80 * 25; i++)
    {
        video_memory[i] = (uint16_t)(' ' | (0x07 << 8));
    }

    for (int i = 0; message[i] != '\0'; i++)
    {
        video_memory[i] = (uint16_t)(message[i] | (0x0F << 8));
    }

    while (1)
    {
        __asm__ volatile ("hlt");
    }
}
