#ifndef PAGE_H
#define PAGE_H

#include <stddef.h>
#include <stdint.h>

#define LIST_SIZE       128               
#define PAGE_SIZE_2MB   (2u * 1024u * 1024u)

struct list_element {
    struct list_element *next;
};


extern struct list_element *free_list;

void init_pfa_list(void);


struct list_element *allocate_physical_pages(unsigned int npages);

void free_physical_pages(struct list_element *ppage_list);

void *page_addr(struct list_element *n);



struct page_directory_entry {
    uint32_t present       : 1;
    uint32_t rw            : 1;
    uint32_t user          : 1;
    uint32_t writethru     : 1;
    uint32_t cachedisabled : 1;
    uint32_t accessed      : 1;
    uint32_t pagesize      : 1;
    uint32_t ignored       : 2;
    uint32_t os_specific   : 3;
    uint32_t frame         : 20;
};

struct page {
    uint32_t present  : 1;
    uint32_t rw       : 1;
    uint32_t user     : 1;
    uint32_t accessed : 1;
    uint32_t dirty    : 1;
    uint32_t unused   : 7;
    uint32_t frame    : 20;
};

struct ppage {
    uint32_t physical_addr;
    struct ppage *next;
};

#endif 
