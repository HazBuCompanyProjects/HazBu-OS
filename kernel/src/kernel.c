#include "keyboard.h"

typedef unsigned short uint16_t;

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_COLOR 0x0F

static volatile uint16_t *const vga_buffer =
    (volatile uint16_t *)0xB8000;

static unsigned int terminal_row = 0;
static unsigned int terminal_column = 0;

static uint16_t vga_entry(char character)
{
    return (uint16_t)character |
           ((uint16_t)VGA_COLOR << 8);
}

static void terminal_scroll(void)
{
    for (unsigned int i = 0;
         i < VGA_WIDTH * (VGA_HEIGHT - 1); i++)
    {
        vga_buffer[i] = vga_buffer[i + VGA_WIDTH];
    }

    for (unsigned int i = VGA_WIDTH * (VGA_HEIGHT - 1);
         i < VGA_WIDTH * VGA_HEIGHT; i++)
    {
        vga_buffer[i] = vga_entry(' ');
    }

    terminal_row = VGA_HEIGHT - 1;
}

static void terminal_clear(void)
{
    for (unsigned int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
    {
        vga_buffer[i] = vga_entry(' ');
    }

    terminal_row = 0;
    terminal_column = 0;
}

static void terminal_putchar(char character)
{
    if (character == '\n')
    {
        terminal_column = 0;
        terminal_row++;
    }
    else if (character == '\r')
    {
        terminal_column = 0;
    }
    else if (character == '\b')
    {
        if (terminal_column > 0)
        {
            terminal_column--;

            vga_buffer[
                terminal_row * VGA_WIDTH + terminal_column
            ] = vga_entry(' ');
        }
    }
    else if (character == '\t')
    {
        do
        {
            terminal_putchar(' ');
        }
        while (terminal_column % 4 != 0);
    }
    else if (character >= ' ')
    {
        vga_buffer[
            terminal_row * VGA_WIDTH + terminal_column
        ] = vga_entry(character);

        terminal_column++;

        if (terminal_column >= VGA_WIDTH)
        {
            terminal_column = 0;
            terminal_row++;
        }
    }

    if (terminal_row >= VGA_HEIGHT)
    {
        terminal_scroll();
    }
}

static void terminal_write(const char *text)
{
    while (*text)
    {
        terminal_putchar(*text);
        text++;
    }
}

static char lower_ascii(char character)
{
    if (character >= 'A' && character <= 'Z')
    {
        return character + ('a' - 'A');
    }

    return character;
}

static int string_equals(const char *a, const char *b)
{
    while (*a && *b)
    {
        if (lower_ascii(*a) != lower_ascii(*b))
            return 0;

        a++;
        b++;
    }

    return *a == *b;
}

static int starts_with(const char *text, const char *prefix)
{
    while (*prefix)
    {
        if (lower_ascii(*text) != lower_ascii(*prefix))
            return 0;

        text++;
        prefix++;
    }

    return 1;
}

static void execute_command(const char *command)
{
    if (string_equals(command, "help"))
    {
        terminal_write("Available commands:\n");
        terminal_write("  help       - Show commands\n");
        terminal_write("  about      - About HazBu OS\n");
        terminal_write("  version    - Show OS version\n");
        terminal_write("  clear      - Clear the screen\n");
        terminal_write("  echo TEXT  - Display text\n");
    }
    else if (string_equals(command, "about"))
    {
        terminal_write("HazBu OS\n");
        terminal_write("A custom operating system project.\n");
        terminal_write("Currently under development.\n");
    }
    else if (string_equals(command, "version"))
    {
        terminal_write("HazBu OS version 0.1.0\n");
        terminal_write("Architecture: x86 (32-bit)\n");
    }
    else if (string_equals(command, "clear"))
    {
        terminal_clear();
    }
    else if (starts_with(command, "echo "))
    {
        terminal_write(command + 5);
        terminal_putchar('\n');
    }
    else if (string_equals(command, "echo"))
    {
        terminal_putchar('\n');
    }
    else if (command[0] != '\0')
    {
        terminal_write("Unknown command: ");
        terminal_write(command);
        terminal_putchar('\n');
        terminal_write("Type 'help' for available commands.\n");
    }
}

void kernel_main(void)
{
    char command[128];
    unsigned int command_length = 0;

    terminal_clear();

    terminal_write("Welcome to HazBu OS!\n");
    terminal_write("HazBu OS Kernel v0.1.0\n");
    terminal_write("Type 'help' to see available commands.\n\n");

    terminal_write("HazBu> ");

    while (1)
    {
        char character = keyboard_poll_char();

        if (character == 0)
        {
            __asm__ volatile ("pause");
            continue;
        }

        if (character == '\n')
        {
            terminal_putchar('\n');

            command[command_length] = '\0';

            execute_command(command);

            command_length = 0;

            terminal_write("HazBu> ");
        }
        else if (character == '\b')
        {
            if (command_length > 0)
            {
                command_length--;
                terminal_putchar('\b');
            }
        }
        else if (character >= 32 && character <= 126)
        {
            if (command_length < sizeof(command) - 1)
            {
                command[command_length++] = character;
                terminal_putchar(character);
            }
        }
    }
}
