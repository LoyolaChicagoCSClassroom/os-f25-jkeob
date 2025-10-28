#ifndef MPAGES_H
#define MPAGES_H

#include <stdint.h>
#include "page.h"

void *map_pages(void *vaddr, struct ppage *pglist, struct page_directory_entry *pd);
void loadPageDirectory(struct page_directory_entry *pd);
void enablePaging(void);

#endif

