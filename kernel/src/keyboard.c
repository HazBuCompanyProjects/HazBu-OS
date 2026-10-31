#include "keyboard.h"

static unsigned char shift_pressed = 0;
static unsigned char caps_lock = 0;
static unsigned char extended_key = 0;

static inline unsigned char inb(unsigned short port)
{
    unsigned char value;

    __asm__ volatile (
        "inb %1, %0"
        : "=a"(value)
        : "Nd"(port)
    );

    return value;
}

static const char normal_map[128] = {
    [0x02] = '1',
    [0x03] = '2',
    [0x04] = '3',
    [0x05] = '4',
    [0x06] = '5',
    [0x07] = '6',
    [0x08] = '7',
    [0x09] = '8',
    [0x0A] = '9',
    [0x0B] = '0',
    [0x0C] = '-',
    [0x0D] = '=',
    [0x0E] = '\b',
    [0x0F] = '\t',

    [0x10] = 'q',
    [0x11] = 'w',
    [0x12] = 'e',
    [0x13] = 'r',
    [0x14] = 't',
    [0x15] = 'y',
    [0x16] = 'u',
    [0x17] = 'i',
    [0x18] = 'o',
    [0x19] = 'p',
    [0x1A] = '[',
    [0x1B] = ']',
    [0x1C] = '\n',

    [0x1E] = 'a',
    [0x1F] = 's',
    [0x20] = 'd',
    [0x21] = 'f',
    [0x22] = 'g',
    [0x23] = 'h',
    [0x24] = 'j',
    [0x25] = 'k',
    [0x26] = 'l',

    [0x27] = ';',
    [0x28] = '\'',
    [0x29] = '`',
    [0x2B] = '\\',

    [0x2C] = 'z',
    [0x2D] = 'x',
    [0x2E] = 'c',
    [0x2F] = 'v',
    [0x30] = 'b',
    [0x31] = 'n',
    [0x32] = 'm',

    [0x33] = ',',
    [0x34] = '.',
    [0x35] = '/',
    [0x39] = ' '
};

static const char shift_map[128] = {
    [0x02] = '!',
    [0x03] = '@',
    [0x04] = '#',
    [0x05] = '$',
    [0x06] = '%',
    [0x07] = '^',
    [0x08] = '&',
    [0x09] = '*',
    [0x0A] = '(',
    [0x0B] = ')',
    [0x0C] = '_',
    [0x0D] = '+',
    [0x1A] = '{',
    [0x1B] = '}',
    [0x27] = ':',
    [0x28] = '"',
    [0x29] = '~',
    [0x2B] = '|',
    [0x33] = '<',
    [0x34] = '>',
    [0x35] = '?'
};

char keyboard_poll_char(void)
{
    unsigned char status = inb(0x64);

    if (!(status & 1))
        return 0;

    unsigned char code = inb(0x60);

    if (code == 0xE0) {
        extended_key = 1;
        return 0;
    }

    if (extended_key) {
        extended_key = 0;
        return 0;
    }

    if (code & 0x80) {
        code &= 0x7F;

        if (code == 0x2A || code == 0x36)
            shift_pressed = 0;

        return 0;
    }

    if (code == 0x2A || code == 0x36) {
        shift_pressed = 1;
        return 0;
    }

    if (code == 0x3A) {
        caps_lock ^= 1;
        return 0;
    }

    char character = normal_map[code];

    if (!character)
        return 0;

    if (character >= 'a' && character <= 'z') {
        if (shift_pressed ^ caps_lock)
            character -= 32;
    }
    else if (shift_pressed && shift_map[code]) {
        character = shift_map[code];
    }

    return character;
}
