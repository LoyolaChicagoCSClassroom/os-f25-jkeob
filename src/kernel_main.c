#include <stdint.h>

__attribute__((section(".multiboot"), used, aligned(8)))
const unsigned int multiboot_header[] = {
    0xe85250d6,   // magic
    0,            // arch (i386)
    24,           // total header length (includes end tag)
    -(0xe85250d6 + 0 + 24), // checksum
    0,            // end-tag type
    8             // end-tag size
};

/* ------------------------------------------------------------------
   Read a byte from I/O port
   ------------------------------------------------------------------ */
uint8_t inb (uint16_t _port) {
    uint8_t rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

/* ------------------------------------------------------------------
   Kernel entry point
   ------------------------------------------------------------------ */
void main() {
    unsigned short *vram = (unsigned short*)0xb8000; // VGA text buffer
    const unsigned char color = 7; // gray on black

    // Write "Hi!" to the top-left of the screen
    vram[0] = ((unsigned short)color << 8) | 'H';
    vram[1] = ((unsigned short)color << 8) | 'i';
    vram[2] = ((unsigned short)color << 8) | '!';

    // Halt loop to save CPU when idle
    for (;;) {
        __asm__ __volatile__("hlt");
    }

    // (Optional: keyboard polling — currently unreachable
    // because of the infinite loop above)
    while (1) {
        uint8_t status = inb(0x64);
        if (status & 1) {
            uint8_t scancode = inb(0x60);
            (void)scancode; // silence unused warning
        }
    }
}
