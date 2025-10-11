#include "page.h"
#include <stdio.h>   

static struct list_element arr[LIST_SIZE];

struct list_element *free_list = NULL;

void *page_addr(struct list_element *n) {
    size_t idx = (size_t)(n - &arr[0]);
    uintptr_t pa = (uintptr_t)idx * PAGE_SIZE_2MB;
    return (void *)pa;
}

void init_pfa_list(void) {
    for (int i = 0; i < LIST_SIZE - 1; i++) {
        arr[i].next = &arr[i + 1];
    }
    arr[LIST_SIZE - 1].next = NULL;
    free_list = &arr[0];
}

static struct list_element *pop_free(void) {
    if (!free_list) return NULL;
    struct list_element *n = free_list;
    free_list = free_list->next;
    n->next = NULL;
    return n;
}

struct list_element *allocate_physical_pages(unsigned int npages) {
    if (npages == 0) return NULL;

    struct list_element *head = NULL;
    struct list_element *tail = NULL;

    for (unsigned int k = 0; k < npages; k++) {
        struct list_element *n = pop_free();
        if (!n) break; // out of pages
        if (!head) head = n;
        else       tail->next = n;
        tail = n;
    }
    return head; // may be null if nothing available
}

void free_physical_pages(struct list_element *ppage_list) {
    if (!ppage_list) return;

    struct list_element *tail = ppage_list;
    while (tail->next) tail = tail->next;

    tail->next = free_list;
    free_list = ppage_list;
}
