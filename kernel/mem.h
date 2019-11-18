#include "task.h"

unsigned int kinit_MemMgmt();
void *kmem_allocpagesphys(unsigned long npages, unsigned long flags, uint32 *phys);
void *kmem_allocpages(unsigned long npages, unsigned long flags);
unsigned int kget_PageBit(unsigned long BitNum);
unsigned int kset_PageBit(unsigned int BitNum);
unsigned int kunset_PageBit(unsigned int BitNum);
void * kmap_Page(void *memp, unsigned long npages, unsigned long flags);
void * kunmap_Page(void *memp, unsigned long npages);
unsigned long kmem_free(void *memp, unsigned long npages);
void memtest(void);
void *mtop(void *memp);
void kinit_tmpuser(void);


/*
 * kmem_alloc flags
 */
#define KMEM_KERNEL	0x00000001
#define KMEM_USER	0x00000002
