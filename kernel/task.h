#ifndef _TASK_H_
#define _TASK_H_

#include "types.h"

#define tKERNEL       0
#define tRUNNING      1
#define tREADY        2
#define tSLEEP_PORT   3
#define tSLEEP_IRQ    4
#define tDEAD         5

#define MaxProc 1024

#define SEL_KCODE (1*8)
#define SEL_KDATA (2*8)
#define SEL_KDATA2 (3*8)
#define SEL_UCODE (4*8)
#define SEL_UDATA (5*8)
#define SEL_KTSS (6*8)

typedef struct _aspace {
	uint32 pdir[1024]; /* page directory -- 4k */
	uint32 ptab[1024]; /* page table -- 4k */
	uint32 high[1024]; /* high page table -- 4k (0x80000000 -> )*/
} aspace;

typedef struct TSS32 {
   unsigned short link, __unused0;
   unsigned long esp0;
   unsigned short ss0, __unused1;
   unsigned long esp1;
   unsigned short ss1, __unused2;
   unsigned long esp2;
   unsigned short ss2, __unused3;
   unsigned long cr3, eip, eflags;
   unsigned long eax,ecx,edx,ebx,esp,ebp,esi,edi;
   unsigned short es, __unused4;
   unsigned short cs, __unused5;
   unsigned short ss, __unused6;
   unsigned short ds, __unused7;
   unsigned short fs, __unused8;
   unsigned short gs, __unused9;
   unsigned short ldt, __unused10;
   unsigned short debugtrap, iomapbase;
}TSS;

typedef struct _task {
    TSS tss;
    char iomap[512];
    uint32 tid;
    uint32 rid;
    struct _aspace *addr;
    uint32 flags;
    uint32 sleeping_on;
    uint32 irq;    
}task;

typedef struct _taskq {
	task *t;
	unsigned long prior;
} taskq;


void kinit_gdt(void);
void kinit_ktask(void);
task *task_create(uint32 ip, uint32 sp);
void task_call(task *t);
void task_add(task *t, unsigned long prior);
void task_delete(task *t);
void aspace_map(aspace *a, uint32 phys, uint32 virt, uint32 len, uint32 flags);
aspace *aspace_create(void);

void haltit(void);

#endif
