; Set TabStop to 4

use16
ORG 0h

    JMP Boot
    NOP

    Herald          DB  'GuildOS1'
    nBytesPerSect   DW  0200h
    nSectPerClstr   DB  01h
    nRsvdSect       DW  0001h
    nFATS           DB  02
    nRootDirEnts    DW  00E0h
    nTotalSectors   DW  0B40h
    bMedia          DB  0F0h
    nSectPerFAT     DW  0009h
    nSectPerTrack   DW  0012h
    nHeads          DW  0002h
    nHidden         DD  00000000h
    nTotalSect32    DD  00000000h
    bBootDrive      DB  00h
    ResvdByte       DB  00h
    ExtBootSig      DB  29h
    nOSSectors      DW  0140h
    ResvdWord       DW  0000h
    Volname         DB  'RICH       '
    FatType         DB  'FAT12   '

	APCX	DW 0002h	; This is the Protected Mode Loader for your OS
	APDX	DW 0000h	; I have supplied this.  This entry should
	APES	DW 6000h	; stay the same for you (60000h Linear)
	APBX	DW 0000h	; Offset is Unused
	APSZ	DB 1h
	APSZ2	DB 0

	BPCX	DW 0003h	; This module is for your kernel or Boot Loader
	BPDX	DW 0000h	; It should start at this location after the ProtModeLdr
	BPES	DW 1000h	; Support for longer files will be added later
	BPBX	DW 0000h	; (10000h Linear) Offset is Unused
	BPSZ	DB 7h
	BPSZ2	DB 0

	CPCX	DW 000ah	; This module is for a 1k user program
	CPDX	DW 0000h
	CPES	DW 2000h	; loaded at 20000h linear
	CPBX	DW 0000h	; Offset is Unused
	CPSZ	DB 02h		; 2 sectors (1024 bytes)
	CPSZ2	DB 00h

	DPCX	DW 000ch	; Another module for your kernel
	DPDX	DW 0000h	; More kernel
	DPES	DW 7000h
	DPBX	DW 0000h	; Offset is Unused
	DPSZ	DB 20h
	DPSZ2	DB 00h

	SIZE	DB 0
	SIZE2	DB 0

;
; Start of loader
;
Boot:
    CLI                                     ; Disable Interrupts

    ; Move the stack to 98000h

    MOV     AX, 9000h
    MOV     SS, AX
    MOV     SP, 8000h

    ; Relocate Boot Sector to 90000h

    MOV     AX, 9000h						; Destination Segment
    MOV     ES, AX
    XOR     DI, DI
    MOV     AX, 7C0h                        ; Source Segment
    MOV     DS, AX
    XOR     SI, SI
    MOV     CX, 512                         ; Copy 512 bytes
    REP     MOVSB

    ; Jump to new location
    MOV     AX, 9000h
    PUSH    AX
    MOV     AX, 8eh
    PUSH    AX
    RETF                                    ; Simulate procedure return far 
                                            ; and use items just pushed
                                            ; onto the stack for location

    ; RETF goes to here
    STI                                     ; Turn interrupts back on

    ; Reset floppy drive

    MOV     DL, bBootDrive                  ; Floppy disk (A:)
    XOR     AX, AX
    INT     13h
    JC      BadBoot
	jmp LoadIt
BadBoot:

    MOV     BX, 7h
    MOV     AH, 14
    MOV     AL, '3'
    INT     10h
	INT	16h
	INT	19h

LoadIt:
    MOV     SI, OFFSET MsgLoad
    CALL    PutChars
          
	; Quick and dirty... Robustify later 
	; First part
	mov cx, APCX
	mov dx, APDX
	mov ax, APES
	mov es, ax
	mov bx, APBX
	mov al, APSZ
	mov SIZE, al
	call LoadFile

	; Second part
	mov cx, BPCX
	cmp cx, 0h
	jz Nomore
	mov dx, BPDX
	mov ax, BPES
	mov es, ax
	mov bx, BPBX
	mov al, BPSZ
	mov SIZE, al
	call LoadFile

	; Third part
	mov cx, CPCX
	cmp cx, 0h
	jz Nomore
	mov dx, CPDX
	mov ax, CPES
	mov es, ax
	mov bx, CPBX
	mov al, CPSZ
	mov SIZE, al
	call LoadFile

	; Fourth part
	mov cx, DPCX
	cmp cx, 0h
	jz Nomore
	mov dx, DPDX
	mov ax, DPES
	mov es, ax
	mov bx, DPBX
	mov al, DPSZ
	mov SIZE, al
	call LoadFile

Nomore:
	JMP	Continue

LoadFile:				; IN: CX, DX, ES, BX, SIZE
	push cx				; Contains starting sector and cylinder
	push dx				; Contains Starting head and drive
	mov al, 01h			; Read in 1 Sector
	mov AH, 02h			; Set the 'Read' Function for INT13h
	xor bx, bx			; Set Offset to 0
LoadGo:
	INT 13h
	pop dx
	pop cx

	cmp ah, 0h			; Check for an error
	jz Btwo				; No, continue 

	MOV     BX, 7h			; Display an E for error
	MOV     AH, 14
	MOV     AL, 'E'
	INT     10h
	jmp LoadFile

Btwo:
	mov al, size
	dec al
	mov size, al			; Remove 1 from size
	cmp al, 0h			; Are we done
	jz DoneLoading			; Yes

	mov bx, es			; Increment segment by 512 bytes
	add bx, 20h			; This is offset+200h so segment+20h
	mov es, bx			; Store new value in es

	inc cl				; No, so increment starting sector #
	cmp cl, 13h			; If we are at the end of the track
					; then switch to new one
	jnz LoadFile			; We are not at end so load next sector


	cmp dh, 0h			; Are we on head 0?
	jnz NewCylinder			; If not switch to new cylinder

	mov cl, 1h			; Zero out sector number
	mov dh, 1h			; Switch to head #1
	jmp LoadFile			; Read in next sector

NewCylinder:

	mov bx, cx			; Extract the current cylinder
	shr bl, 6
	xchg bl, bh
	add bx, 1h			; Add 1 to the cylinder number
	xchg bl, bh
	shl bl, 6
	mov cx, bx			; Place new cylinder into cx

	mov cl, 1h			; Zero out sector number
	mov dh, 0h			; Switch to head #0
	inc ch				; Increment cylinder number
	jmp LoadFile			; Read in next sector

DoneLoading:
	ret


PutChars:
    LODSB
    OR      AL, AL
    JZ      SHORT Done
    MOV     AH, 0Eh
    MOV     BX, 0007
    INT     10h
    JMP     SHORT PutChars
Done:
    RET


Continue:
    MOV     BX, 7h
    MOV     AH, 14
    MOV     AL, '6'
    INT     10h

	
	mov ax, APES
	push ax
	mov ax, APBX
	push ax
	retf



MsgLoad     DB 0Dh, 0Ah, 'Loading', 00h

ORG     1FEh
Signature   DW  0AA55h
