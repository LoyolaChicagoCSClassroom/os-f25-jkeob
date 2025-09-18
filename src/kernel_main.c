#include <stdint.h>
#include "console.h"
#include "rprintf.h"   // header for your rprintf functions

#define MULTIBOOT2_HEADER_MAGIC         0xe85250d6

const unsigned int multiboot_header[]  __attribute__((section(".multiboot"))) = {MULTIBOOT2_HEADER_MAGIC, 0, 16, -(16+MULTIBOOT2_HEADER_MAGIC), 0, 12};

uint8_t inb (uint16_t _port) {
    uint8_t rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

/* ------------------------------------------------------------------
   Kernel entry point
   ------------------------------------------------------------------ */
void main() {
    puts("Hi!\n");
    puts("Keyboard polling demo starting...\n");

    while (1) {
        // poll keyboard
        uint8_t status = inb(0x64);
        if (status & 1) {
            uint8_t scancode = inb(0x60);
            esp_printf(console_putc, "Key pressed: %02x\n", scancode);
        }

        // crude delay loop (~1 second depending on CPU speed/QEMU)
        for (volatile unsigned long i = 0; i < 100000000; i++) {
            // just waste time
        }

        // heartbeat message
        puts("tick...\n");

    }
}
