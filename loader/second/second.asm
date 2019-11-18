SegDesc MACRO
	dw (#3 AND 0FFFF)
	dw (#2 AND 0FFFF)
	db #1 AND 0FFh
	db #4
	db #5 OR #6
	db #1 SHR 8
#EM

use16
ORG 0h

Boot:

	mov bx, 7h
	mov ah, 14
	mov al, 'A'
	int 10h

	mov ax, cs
	mov ds, ax
	mov es, ax

A20Address:
    ; Set A20 Address line here

    CLI
	CALL enableA20
    STI
	JMP Continue
enableA20:
        call enableA20o1
        jnz short enableA20done
        mov al,0d1h
        out 64h,al
        call enableA20o1
        jnz short enableA20done
        mov al,0dfh
        out 60h,al
enableA20o1:
        mov ecx,20000h
enableA20o1l:
        jmp short $+2
        in al,64h
        test al,2
        loopnz enableA20o1l
enableA20done:
        ret
	


Continue:
	mov bx, 7h
	mov ah, 14
	mov al, 'B'
	int 10h

    ; Set Global Descriptor Table here, this must be done PRIOR to Prot Mode

    LGDT    CS:[GDT]

    ; Set Protected Mode here
   
CLI

    MOV     EAX, CR0
    OR      EAX, 1
    MOV     CR0, EAX
    JMP     $+2

	db		066h, 0eah
	dw		offset prot, 06h
	dw		08h

prot:	use32
	; Set up segments
	mov ebx, 10h
	mov ds, bx
	mov es, bx
	mov fs, bx
	mov gs, bx

mov ebx, 0b8000h
mov byte ptr [ebx], 25h
mov byte ptr [ebx + 1], 07h
mov byte ptr [ebx + 2], 07h
mov byte ptr [ebx + 3], 64h

	; Set up stack segment
	mov ebx, 18h
	mov ss, bx
	mov eax, 90000h - 4
	mov esp, eax


	push eax
	mov ebx, 10080h
	call ebx

db 0bdh

GDTlen equ 6*8
GDT	dw 0ffffh
	dd 60099h
	dw 0

	SegDesc 00, 0000h, 0ffff,10011010xb, 0cfh, 0	; kCS (08h) CPL0
	SegDesc 00, 0000h, 0ffff,10010010xb, 0cfh, 0	; kDS (10h)
	SegDesc 00, 0000h, 0ffff,10010010xb, 0cfh, 0	; kSS (18h)
	SegDesc 00, 0000h, 0ffff,10011010xb, 8fh, 0	; kCS16 (20h)
	SegDesc 00, 0000h, 0ffff,10010010xb, 8fh, 0	; kSS16 (28h)
	SegDesc 00, 0000h, 0ffff,11111010xb, 0cfh, 0	; aCS
	SegDesc 00, 0000h, 0ffff,11110010xb, 0cfh, 0	; aDS
	SegDesc 00, 0000h, 0ffff,11110010xb, 0cfh, 0	; aSS

MsgLoad     DB 0Dh, 0Ah, 'Loading', 00h

ORG     1FEh
Signature   DW  0AA55h
