#include "task.h"
#include "mem.h"
#include "i386.h"
#include "conio.h"

char *gdt;
task *KernelTask;
task *CurrentTask;
unsigned long CurrentTaskNum;
uint32 _cr3;
static uint32 nextentry = 48;
taskq TaskList[MaxProc];

extern uint32 *flat;

uint32
getgdtentry(void) {
	nextentry += 8;
	return nextentry;
}

void
kinit_task(void) {

	kinit_gdt();
	kinit_ktask();
	
}

void
kinit_ktask() {
	TSS *ktss;
	unsigned long i;

	for(i = 0; i < MaxProc; i++)
		TaskList[i].t = 0;

	ktss = (TSS *) kmem_allocpages(1, 3);
	KernelTask = (task *) ktss;
	KernelTask->flags = tKERNEL;
	CurrentTask = KernelTask;
	CurrentTaskNum = 0;

	i386SetSegment(gdt + SEL_KTSS,
			(uint32) ktss, 104,
			i386rPRESENT | i386rDPL0 | i386rTSS,
			0);

	/* Get cr3 (page table address to store in tss */
	asm("mov %%cr3, %0":"=r" (_cr3));


	/* Setup kernel TSS */
	KernelTask->tid = SEL_KTSS;
	ktss->ss0 = SEL_KDATA;
	ktss->esp1 = ktss->esp2 = ktss->ss1 = ktss->ss2 = 0;
	KernelTask->addr =  (aspace *) flat;


	ktss->cs = SEL_KCODE;
	ktss->ss = ktss->ds = ktss->es = ktss->fs = ktss->gs = SEL_KDATA;
	ktss->ldt = ktss->debugtrap = ktss->iomapbase = 0;
	ktss->cr3 = _cr3;
	i386ltr(SEL_KTSS);

	task_add(KernelTask, 0);

	kprintf("Allocated ktss at: %x\n", ktss);
	mtop(ktss);

}

void
kinit_gdt() {
	gdt = (char *) kmem_allocpages(1, 3);

	kprintf("Allocated gdt at: %x\n", gdt);
	mtop(gdt);


	i386SetSegment(gdt + SEL_KCODE,
			0, 0xFFFFF,
			i386rPRESENT | i386rDPL0 | i386rCODE,
			i386g4K | i386g32BIT);

	i386SetSegment(gdt + SEL_KDATA,
			0, 0xFFFFF,
			i386rPRESENT | i386rDPL0 | i386rDATA,
			i386g4K | i386g32BIT);

	i386SetSegment(gdt + SEL_KDATA2,
			0, 0xFFFFF,
			i386rPRESENT | i386rDPL0 | i386rDATA,
			i386g4K | i386g32BIT);

	i386SetSegment(gdt + SEL_UCODE,
			0, 0xFFFFF,
			i386rPRESENT | i386rDPL3 | i386rCODE,
			i386g4K | i386g32BIT);

	i386SetSegment(gdt + SEL_UDATA,
			0, 0xFFFFF,
			i386rPRESENT | i386rDPL3 | i386rDATA,
			i386g4K | i386g32BIT);


	i386lgdt((uint32) gdt, 1024/8);

}

void
dump_task(task *t) {

	kprintf("task: 0x%x\n", t);
	kprintf("tid: 0x%x    esp0: 0x%x    ss0: 0x%x \n",
		t->tid, t->tss.esp0, t->tss.ss0);
	kprintf("eax: 0x%x    ebx: 0x%x    ecx: 0x%x    ed: 0x%x\n",
		t->tss.eax, t->tss.ebx, t->tss.ecx, t->tss.edx);
	kprintf("esp1: 0x%x    esp2: 0x%x    ss1: 0x%x    ss2: 0x%x\n",
		t->tss.esp1, t->tss.esp2, t->tss.ss1, t->tss.ss2);
	kprintf("cr3: 0x%x    eip: 0x%x    eflags: 0x%x    esp: 0x%x\n",
		t->tss.cr3, t->tss.eip, t->tss.eflags, t->tss.esp);
	kprintf("cs: 0x%x    ds: 0x%x    es: 0x%x    fs: 0x%x    gs: 0x%x\n",
		t->tss.cs, t->tss.ds, t->tss.es, t->tss.fs, t->tss.gs);
	kprintf("ldt: 0x%x    debugtrap: 0x%x    iomap: 0x%x    pdir: 0x%x\n",
		t->tss.ldt, t->tss.debugtrap, t->tss.iomapbase, t->addr);
}

task *
task_create(uint32 ip, uint32 sp) {
	task *t = (task *)kmem_allocpages(1, 3);
	aspace *a = aspace_create();
	unsigned long i;
	aspace *tmp = (aspace *)_cr3;

	for(i = 0; i < 1024; i++)
		a->ptab[i] = i*4096 | 7;


	t->tid = getgdtentry();
	t->tss.esp0 = (uint32) ( ((char *) t) + 4092 );
	t->tss.ss0 = SEL_KDATA;
	t->tss.eax = 0xDEADBEEF;
	t->tss.ebx = 0xDEADBEEF;
	t->tss.ecx = 0xDEADBEEF;
	t->tss.edx = 0xDEADBEEF;
	t->tss.esp1 = t->tss.esp2 = t->tss.ss1 = t->tss.ss2 = 0;
	t->tss.cr3 = (((uint32) a->pdir[0]) & 0xFFFFFF00) - 4096;
	t->tss.eip = (uint32) ip;
	t->tss.eflags = 0x4202;
	t->tss.esp = (uint32) sp;
	t->tss.cs = SEL_KCODE;
	t->tss.ds = t->tss.es = t->tss.ss = 
		t->tss.fs = t->tss.gs = SEL_KDATA;
	t->tss.ldt = t->tss.debugtrap = t->tss.iomapbase = 0;
	t->addr = a;



	i386SetSegment(gdt + t->tid, (uint32) t, 104,
		i386rPRESENT | i386rDPL3 | i386rTSS, 
		0);

	return t;
}

void
task_call(task *t) {
	uint32 sel[2];
	sel[1] = t->tid;
	__asm__("lcall %0; clts"::"m" (*sel));
}

aspace *
aspace_create(void) {
	int i;
	uint32 phys;

	aspace *a = (aspace *) kmem_allocpagesphys(2, 7, &phys);
	
	for(i = 0; i < 1024; i++) {
		a->pdir[i] = 0;
		a->ptab[i] = 0;
	}

	/* Map page table into page directory */
	a->pdir[0] = ((uint32) phys + 4096) | 7;
	/* Map kernels high page table into page directory */
	a->pdir[512] = (_cr3 + 2*4096) | 7;

	return a;
}

void
aspace_map(aspace *a, uint32 phys, uint32 virt, uint32 len, uint32 flags) {
	int i;

	for(i = 0; i < len; i++)
		a->ptab[virt + i] = (phys + i) * 4096 | flags;
}


void
task_add(task *t, unsigned long prior) {
	unsigned long i = 0;

	while(i != MaxProc) {
		if(TaskList[i].t == 0) {
kprintf("task_add: Added a task at #%d p:0x%x\n", i, t);
			TaskList[i].t = t;
			TaskList[i].prior = prior;
			return;
		}
		i++;
	}

	kprintf("TASK: ERROR the task list is full\n");
}

void
task_delete(task *t) {
	unsigned long i = 0;

	while(i != MaxProc) {
		if(TaskList[i].t == t) {
			TaskList[i].t = 0;
			return;
		}
		i++;
	}
}

void
swtch() {
	unsigned long i;

//kprintf("SWTCH: Called. In Task: #0x%x  p:0x%x\n", CurrentTaskNum, CurrentTask);
Again:
	if(CurrentTaskNum == (MaxProc - 1))
		CurrentTaskNum = 0;
	else
		CurrentTaskNum += 1;

	if(((uint32)TaskList[CurrentTaskNum].t) == 0) goto Again;


	CurrentTask = TaskList[CurrentTaskNum].t;


	i386SetSegment(gdt + CurrentTask->tid,
		(uint32) CurrentTask, 104,
		i386rPRESENT | i386rDPL3 | i386rTSS, 
		0);

//kprintf("SWTCH: Switching to Task: #0x%x p:0x%x\n", CurrentTaskNum, CurrentTask);
//kprintf("SWTCH: Calling Task\n");

//dump_task(CurrentTask);

	task_call(CurrentTask);

	asm("pushf ; pop %%eax ; andl $0xBFFF, %%eax ; push %%eax ;; popf":::"ax");
}

void
haltit(void) {
bloah:
	goto bloah;
}
