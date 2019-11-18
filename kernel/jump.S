# $Id: jump.S,v 1.2 1997/10/23 03:38:52 swetland Exp $

.text
.globl __syscall20
.globl __syscall21
.globl __syscall22
.globl __syscall23
.globl __syscall24
.globl __syscall25
.globl __syscall26
.globl __syscall27
.globl __syscall28
.globl __syscall29
.globl __syscall2A
.globl __null_irq
.globl __timer_irq
.globl __kbd_irq
.globl __ex0
.globl __ex1
.globl __ex2
.globl __ex3
.globl __ex4
.globl __ex5
.globl __ex6
.globl __ex7
.globl __ex8
.globl __ex9
.globl __ex10
.globl __ex11
.globl __ex12
.globl __ex13
.globl __ex14
.globl __ex15
.globl __ex16
.globl __ex17
.globl __ex18
.globl __irq1
.globl __irq2
.globl __irq3
.globl __irq4
.globl __irq5
.globl __irq6
.globl __irq7
.globl __irq8
.globl __irq9
.globl __irq10
.globl __irq11
.globl __irq12
.globl __irq13
.globl __irq14
.globl __irq15
.globl __enableirq

__enableirq:
	sti
	ret
	
__null_irq:
	movb $32, %al
	outb %al, $32
	iret

#__syscall20:
#	pusha
#	call syscall20
#	popa
#	iret

#__syscall21:
#	pusha
#	call syscall21
#	popa
#	iret

__syscall22:
	pusha
	call syscall22
	popa
	iret

#__syscall23:
#	pusha
#	call syscall23
#	popa
#	iret

#__syscall24:
#	pusha
#	call syscall24
#	popa
#	iret

#__syscall25:
#	pusha
#	call syscall25
#	popa
#	iret

#__syscall26:
#	pusha
#	call syscall26
#	popa
#	iret
	
#__syscall27:
#	pusha
#	call syscall27
#	popa
#	iret
	
#__syscall28:
#	pusha
#	call syscall28
#	popa
#	iret

#__syscall29:
#	pusha
#	call syscall29
#	popa
#	iret

#__syscall2A:
#	pusha
#	call syscall2A
#	popa
#	iret

	
__timer_irq:
	pusha
	movb $32, %al
	outb %al, $32
	call timer_irq
	popa
	iret

__ex0:
	pusha
	pushl $0
	jmp _fault0
__ex1:
	pusha
	pushl $1
	jmp _fault0
__ex2:
	pusha
	pushl $2
	jmp _fault0
__ex3:
	pusha
	pushl $3
	jmp _fault0
__ex4:
	pusha
	pushl $4
	jmp _fault0
__ex5:
	pusha
	pushl $5
	jmp _fault0
__ex6:
	pusha
	pushl $6
	jmp _fault0
__ex7:
	pusha
	pushl $7
	jmp _fault0
__ex8:
	pusha	
	pushl $8
	jmp _fault1
__ex9:
	pusha
	pushl $9
	jmp _fault0
__ex10:
	pusha
	pushl $10
	jmp _fault1
__ex11:
	pusha
	pushl $11
	jmp _fault1
__ex12:
	pusha
	pushl $12
	jmp _fault1
__ex13:
	pusha
	pushl $13
	jmp _fault1
__ex14:
	pusha
	pushl $14
	jmp _fault1
__ex15:
	pusha
	pushl $15
	jmp _fault0
__ex16:
	pusha
	pushl $16
	jmp _fault0
__ex17:
	pusha
	pushl $17
	jmp _fault0
__ex18:
	pusha
	pushl $18
	jmp _fault0

__irq1:	
	pushl $1
	jmp __irq
__irq2:	
	pushl $2
	jmp __irq
__irq3:	
	pushl $3
	jmp __irq
__irq4:	
	pushl $4
	jmp __irq
__irq5:	
	pushl $5
	jmp __irq
__irq6:	
	pushl $6
	jmp __irq
__irq7:	
	pushl $7
	jmp __irq
__irq8:	
	pushl $8
	jmp __irq
__irq9:	
	pushl $9
	jmp __irq	
__irq10:	
	pushl $10
	jmp __irq	
__irq11:	
	pushl $11
	jmp __irq	
__irq12:	
	pushl $12
	jmp __irq	
__irq13:	
	pushl $13
	jmp __irq	
__irq14:	
	pushl $14
	jmp __irq	
__irq15:	
	pushl $15
	jmp __irq
	
__irq:
	pusha

#	call irq_dispatch
		
	movb $32, %al
	outb %al, $32
	
	popa
	add $4,%esp
	iret
			
_fault0:
	call fault
	add $4,%esp
	popa
	iret
_fault1:
	call faultE
	add $4,%esp
	popa
	add $4,%esp
	iret

