#include <stdint.h>
#include "console.h"
#include "rprintf.h"   // header for your rprintf functions
#include "page.h"


#define MULTIBOOT2_HEADER_MAGIC         0xe85250d6

const unsigned int multiboot_header[]  __attribute__((section(".multiboot"))) = {MULTIBOOT2_HEADER_MAGIC, 0, 16, -(16+MULTIBOOT2_HEADER_MAGIC), 0, 12};

uint8_t inb (uint16_t _port) {
    uint8_t rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}
unsigned char keyboard_map[128] =
{
   0,  27, '1', '2', '3', '4', '5', '6', '7', '8',     /* 9 */
 '9', '0', '-', '=', '\b',     /* Backspace */
 '\t',                 /* Tab */
 'q', 'w', 'e', 'r',   /* 19 */
 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* Enter key */
   0,                  /* 29   - Control */
 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',     /* 39 */
'\'', '`',   0,                /* Left shift */
'\\', 'z', 'x', 'c', 'v', 'b', 'n',                    /* 49 */
 'm', ',', '.', '/',   0,                              /* Right shift */
 '*',
   0,  /* Alt */
 ' ',  /* Space bar */
   0,  /* Caps lock */
   0,  /* 59 - F1 key ... > */
   0,   0,   0,   0,   0,   0,   0,   0,  
   0,  /* < ... F10 */
   0,  /* 69 - Num lock*/
   0,  /* Scroll Lock */
   0,  /* Home key */
   0,  /* Up Arrow */
   0,  /* Page Up */
 '-',
   0,  /* Left Arrow */
   0,  
   0,  /* Right Arrow */
 '+',
   0,  /* 79 - End key*/
   0,  /* Down Arrow */
   0,  /* Page Down */
   0,  /* Insert Key */
   0,  /* Delete Key */
   0,   0,   0,  
   0,  /* F11 Key */
   0,  /* F12 Key */
   0,  /* All other keys are undefined */
};
/* ------------------------------------------------------------------
   Kernel entry point
   ------------------------------------------------------------------ */

void main() {
    puts("Hi!\n");
    puts("Keyboard polling demo starting...\n");

    init_pfa_list();

    struct list_element *a = allocate_physical_pages(3);
    for (struct list_element *t = a; t; t = t->next) {
        esp_printf(console_putc, "alloc phys=%08x\n",
                   (uint32_t)(uintptr_t)page_addr(t));   // 32-bit target
    }

    free_physical_pages(a);

    struct list_element *b = allocate_physical_pages(LIST_SIZE + 10);
    esp_printf(console_putc, "alloc_all head phys=%08x\n",
               b ? (uint32_t)(uintptr_t)page_addr(b) : 0u);
    free_physical_pages(b);

    while (1) {
        uint8_t status = inb(0x64);
        if (status & 1) {
<<<<<<< HEAD
            uint8_t scancode = inb(0x60);
            if (scancode > 128) {
                continue;
            }
            esp_printf(console_putc, "Key pressed: %c\n", keyboard_map[scancode]);
}        

}
=======
            uint8_t sc = inb(0x60);
            if (sc <= 128) esp_printf(console_putc, "Key pressed: %c\n", keyboard_map[sc]);
        }
    }
>>>>>>> a30c76c (add page allocator (page.c/.h), hook into kernel_main, print phys addrs)
}
