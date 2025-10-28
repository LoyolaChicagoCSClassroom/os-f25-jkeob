#include <stdint.h>
#include "console.h"
#include "rprintf.h"   // header for your rprintf functions
#include "page.h"
#include "mpages.h"

#define MULTIBOOT2_HEADER_MAGIC 0xe85250d6

const unsigned int multiboot_header[] __attribute__((section(".multiboot"))) = {
    MULTIBOOT2_HEADER_MAGIC, 0, 16, -(16 + MULTIBOOT2_HEADER_MAGIC), 0, 12
};

uint8_t inb(uint16_t _port) {
    uint8_t rv;
    __asm__ __volatile__("inb %1, %0" : "=a"(rv) : "dN"(_port));
    return rv;
}

unsigned char keyboard_map[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', /* 9 */
    '9', '0', '-', '=', '\b',                      /* Backspace */
    '\t',                                          /* Tab */
    'q', 'w', 'e', 'r',                            /* 19 */
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',  /* Enter key */
    0,                                             /* 29 - Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', /* 39 */
    '\'', '`', 0,                                  /* Left shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n',            /* 49 */
    'm', ',', '.', '/', 0,                         /* Right shift */
    '*',
    0,  /* Alt */
    ' ', /* Space bar */
    0,  /* Caps lock */
    0, 0, 0, 0, 0, 0, 0, 0,  /* F1-F8 */
    0, 0,  /* F9, F10 */
    0,  /* Num lock */
    0,  /* Scroll lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
    '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
    '+',
    0,  /* End key */
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0, 0, 0,
    0,  /* F11 */
    0,  /* F12 */
    0   /* Undefined */
};



/* ------------------------------------------------------------------
   Kernel entry point
   ------------------------------------------------------------------ */
void main() {
    puts("Hi!\n");
    puts("Keyboard polling demo starting...\n");

    // ------------------------------------------------------------
    // 1. Initialize page frame allocator and test allocations
    // ------------------------------------------------------------
    init_pfa_list();

    struct list_element *a = allocate_physical_pages(3);
    for (struct list_element *t = a; t; t = t->next) {
        esp_printf(console_putc, "alloc phys=%08x\n",
                   (uint32_t)(uintptr_t)page_addr(t)); // 32-bit target
    }

    free_physical_pages(a);

    struct list_element *b = allocate_physical_pages(LIST_SIZE + 10);
    esp_printf(console_putc, "alloc_all head phys=%08x\n",
               b ? (uint32_t)(uintptr_t)page_addr(b) : 0u);
    free_physical_pages(b);

    // ------------------------------------------------------------
    // 2. Setup minimal identity-mapped paging
    // ------------------------------------------------------------
    extern struct page_directory_entry pd[];  // defined in mpages.c
    void init_identity_map(void);              // function from mpages.c

    puts("Setting up identity paging...\n");

    init_identity_map();       // fill page table to map 0x0–0x3FFFFF
    loadPageDirectory(pd);     // move page directory base to CR3
    enablePaging();            // set PG + PE bits in CR0

    puts("Paging enabled!\n");

    // ------------------------------------------------------------
    // 3. Keyboard polling demo
    // ------------------------------------------------------------
    while (1) {
        uint8_t status = inb(0x64);
        if (status & 1) {
            uint8_t scancode = inb(0x60);
            if (scancode > 128) {
                continue;
            }
            esp_printf(console_putc, "Key pressed: %c\n", keyboard_map[scancode]);
        }
    }
}
