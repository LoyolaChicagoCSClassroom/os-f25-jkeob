#include <stdint.h>
#include "console.h"

#define VGA_MEM      ((volatile uint8_t*)0xB8000)
#define VGA_COLS     80
#define VGA_ROWS     25
#define ATTR_DEFAULT 0x07  // light grey on black

static int cursor = 0;     // 0..(80*25-1)

static void scroll(void) {
    if (cursor < VGA_COLS * VGA_ROWS) {
        return;
    } else {
        for (int row = 1; row < VGA_ROWS; row++) {
            for (int col = 0; col < VGA_COLS; col++) {
                VGA_MEM[(row - 1) * VGA_COLS * 2 + col * 2] =
                    VGA_MEM[row * VGA_COLS * 2 + col * 2];
                VGA_MEM[(row - 1) * VGA_COLS * 2 + col * 2 + 1] =
                    VGA_MEM[row * VGA_COLS * 2 + col * 2 + 1];
            }
        }
        for (int col = 0; col < VGA_COLS; col++) {
            int off = (VGA_ROWS - 1) * VGA_COLS * 2 + col * 2;
            VGA_MEM[off]     = ' ';
            VGA_MEM[off + 1] = ATTR_DEFAULT;
        }
        cursor = (VGA_ROWS - 1) * VGA_COLS;
    }
}

static void newline(void) {
    cursor = (cursor / VGA_COLS + 1) * VGA_COLS;
    scroll();
}

/**
 * console_putc - print a single character to the VGA text buffer
 * Returns the character (int) so it matches printf-style expectations
 */
int console_putc(int ch) {
    if (ch == '\n') {
        newline();
        return ch;
    }
    if (ch == '\r') {
        return ch;
    }

    int off = cursor * 2;
    VGA_MEM[off] = (uint8_t)ch;
    VGA_MEM[off + 1] = ATTR_DEFAULT;
    cursor++;
    scroll();
    return ch;
}

/**
 * putc - stdio-like wrapper (just calls console_putc)
 */
int putc(int ch) {
    return console_putc(ch);
}

/**
 * puts - print a null-terminated string to VGA
 */
void puts(const char *s) {
    while (*s) {
        console_putc(*s++);
    }
}
