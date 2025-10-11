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

#endif 
