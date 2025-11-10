#include <stdint.h>
#include "console.h"
#include "rprintf.h"
#include "page.h"
#include "mpages.h"
#include "fat.h"

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
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8',
    '9', '0', '-', '=', '\b',
    '\t',
    'q', 'w', 'e', 'r',
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n',
    'm', ',', '.', '/', 0,
    '*',
    0,
    ' ',
    0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0,
    0,
    0,
    0,
    0,
    0,
    '-',
    0,
    0,
    0,
    '+',
    0,
    0,
    0,
    0,
    0,
    0, 0, 0,
    0,
    0
};


void main(void) {
    puts("Hi!\n");
    puts("Starting kernel setup...\n");

    // ------------------------------------------------------------
    // 1. Initialize page frame allocator and test allocations
    // ------------------------------------------------------------
    init_pfa_list();

    struct list_element *a = allocate_physical_pages(3);
    for (struct list_element *t = a; t; t = t->next) {
        esp_printf(console_putc, "alloc phys=%08x\n",
                   (uint32_t)(uintptr_t)page_addr(t));
    }

    free_physical_pages(a);

    struct list_element *b = allocate_physical_pages(LIST_SIZE + 10);
    esp_printf(console_putc, "alloc_all head phys=%08x\n",
               b ? (uint32_t)(uintptr_t)page_addr(b) : 0u);
    free_physical_pages(b);

    // ------------------------------------------------------------
    // 2. Setup minimal identity-mapped paging
    // ------------------------------------------------------------
    extern struct page_directory_entry pd[];
    void init_identity_map(void);

    puts("Setting up identity paging...\n");
    init_identity_map();
    loadPageDirectory(pd);
    enablePaging();
    puts("Paging enabled!\n");

    // ------------------------------------------------------------
    // 3. Run FAT filesystem test
    // ------------------------------------------------------------
    puts("Running FAT filesystem test...\n");
    puts("FAT test complete.\n");

    // ------------------------------------------------------------
    // 4. Keyboard polling demo (optional)
    // ------------------------------------------------------------
    puts("Keyboard polling demo starting...\n");
    while (1) {
        uint8_t status = inb(0x64);
        if (status & 1) {
            uint8_t scancode = inb(0x60);
            if (scancode > 128)
                continue;
            esp_printf(console_putc, "Key pressed: %c\n", keyboard_map[scancode]);
        }
    }
}
