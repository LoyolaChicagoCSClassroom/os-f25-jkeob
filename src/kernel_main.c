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
void kernel(void) {
    // show that your kernel + rprintf are alive
    puts("Hi!\n");
    puts("Keyboard polling demo starting...\n");

    // main keyboard loop
    while (1) {
        uint8_t status = inb(0x64);   // read status register
        if (status & 1) {             // if output buffer full
            uint8_t scancode = inb(0x60);   // read key from data port
            puts("Key pressed: %02x\n", scancode);
        }

        __asm__ __volatile__("hlt");  // idle until next interrupt/IO
    }
}
