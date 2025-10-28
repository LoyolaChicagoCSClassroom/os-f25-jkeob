#include <stdint.h>
#include "page.h"
#include "mpages.h"

// one 4 KB-aligned directory and table
struct page_directory_entry pd[1024] __attribute__((aligned(4096)));
struct page pt[1024] __attribute__((aligned(4096)));

void init_identity_map() {
    // map 0x00000000–0x003FFFFF (4 MB) 1:1
    for (uint32_t i = 0; i < 1024; i++) {
        pt[i].present = 1;
        pt[i].rw = 1;
        pt[i].user = 0;
        pt[i].frame = i;  // frame = physical page number
    }

    pd[0].present = 1;
    pd[0].rw = 1;
    pd[0].user = 0;
    pd[0].frame = ((uint32_t)&pt) >> 12;
}

void loadPageDirectory(struct page_directory_entry *pd) {
    __asm__ __volatile__("mov %0, %%cr3" :: "r"(pd) :);
}

void enablePaging() {
    __asm__ __volatile__(
        "mov %cr0, %eax\n"
        "or  $0x80000001, %eax\n"
        "mov %eax, %cr0"
    );
}
