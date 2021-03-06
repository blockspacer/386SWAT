;' $Header:   P:/PVCS/386SWAT/SWAT_COM.INV   1.20   10 Aug 1998 11:00:54   BOB  $
;
; (C) Copyright 1987-1999 Qualitas, Inc.  All rights reserved.
;
; SWAT_COM.INC
;
; Common structures and macros for 386SWAT
;

; Common structures for 386SWAT

BACK_STR struc

BACK_GDT df	?		; Limit and base address for GDT
BACK_IDT df	?		; ...			     IDT
BACK_LDT df	?		; ...			     LDT
BACK_TR df	?		; ...			     TR

BACK_SS df	?		; Segment/selector and linear address
BACK_GS df	?
BACK_FS df	?
BACK_ES df	?
BACK_DS df	?
BACK_CS df	?

BACK_STR ends

@BPBACK equ	(size BACK_STR)


FORW_STR struc

FORW_EDI dd	?		; Caller's EDI
FORW_ESI dd	?		;	   ESI
FORW_EBP dd	?		;	   EBP
FORW_ESP0 dd	?		;	   ESP0
FORW_EBX dd	?		;	   EBX
FORW_EDX dd	?		;	   EDX
FORW_ECX dd	?		;	   ECX
FORW_EAX dd	?		;	   EAX
FORW_RET dq	?		;	   Return CS:EIP w/filler
FORW_EIP dd	?		;	   EIP
FORW_CS dw	?		;	   CS
FORW_LDT dw	?		;	   LDT
FORW_EFL dd	?		;	   EFL
FORW_ESP dd	?		;	   ESP
FORW_SS dw	?,0		;	   SS
FORW_ES dw	?,0		;	   ES
FORW_DS dw	?,0		;	   DS
FORW_FS dw	?,0		;	   FS
FORW_GS dw	?,0		;	   GS

FORW_STR ends


VMINT_STR struc

VMINT_EIP dd	?		; 00:  Old EIP
VMINT_CS dw	?,?		; 04:  ... CS
VMINT_EFL dd	?		; 08:  ... EFL
VMINT_ESP dd	?		; 0C:  ... ESP3
VMINT_SS dw	?,?		; 10:  ... SS
VMINT_ES dw	?,?		; 14:  ... ES
VMINT_DS dw	?,?		; 18:  ... DS
VMINT_FS dw	?,?		; 1C:  ... FS
VMINT_GS dw	?,?		; 20:  ... GS

VMINT_STR ends


@NCOLS	equ	80		; # screen columns
@NROWS	equ	25		; ...	   rows
@SCRSIZE equ	@NROWS*@NCOLS*2 ; # bytes on screen

@FBROWS_NROWS equ @NROWS-3	; Number of rows for file browser screen
				; (2 header lines, 1 footer)

; Window structure

W_STR	struc

SROW	dw	?		; Starting row
SCOL	dw	?		; ...	   column
NROW	dw	?		; # rows
NCOL	dw	?		; # columns

W_STR	ends


; Last screen buffer table structure

PLSTBUF_STR struc

PLSTBUF dd	?		; Offset in DGROUP of this buffer

PLSTBUF_STR ends


; IRET frame structure

IRET_STR struc

IRET_IP dw	?		; IP
IRET_CS dw	?		; CS
IRET_FL dw	?		; FL

IRET_STR ends


IRETD_STR struc

IRETD_EIP dd	?		; EIP
IRETD_CS  dw	?,?		; CS w/filler
IRETD_EFL dd	?		; EFL

IRETD_STR ends


; Equates for DSP_STATE (index into DISP_ACT, etc.)

@DSP_IREGS equ	0		; Instructions
@DSP_GDT   equ	1		; GDT
@DSP_IDT   equ	2		; IDT
@DSP_LDT   equ	3		; LDT
@DSP_TSS   equ	4		; TSS
@DSP_PTE   equ	5		; PTE
@DSP_PDE   equ	6		; PDE
@DSP_MEM   equ	7		; Memory
@DSP_DRn   equ	8		; Debug registers
@DSP_SRCH  equ	9		; Search
@DSP_BC    equ	10		; Code breakpoints
@DSP_LSCR  equ	11		; Last screens
@DSP_SYMTAB equ 12		; Symbol table
@DSP_FBROWS equ 13		; File browser
@DSP_ERRLOG equ 14		; Error log
@DSP_MAC   equ	15		; MAC entries
@DSP_IVT   equ	16		; IVT ...
@DSP_WGH   equ	17		; WGH ...
@DSP_TDB   equ	18		; TDB ...
@DSP_MDB   equ	19		; MDB ...
@DSP_DLG   equ	20		; DLG
@DSP_BP    equ	21		; Page breakpoints
@DSP_MMX   equ	22		; MMX registers

@DSP_CNT equ	23		; # @DSP_xxx values


; Equates for TSSTYPE

@TSS286 equ	0		; Display as 286 TSS
@TSS386 equ	1		; ...	     386


; Equates for DLGTYPE

@DLGA	equ	0		; Display as Dialog-A (16-bit)
@DLGW	equ	1		; ...	     Dialog-W (32-bit)


; Macro for INT 41h so we can call it without having hooked it

INT41	macro
	local	L1,L2

	test	SWATINI.MD_ATTR,@MD_WIN3 ; Running under Windows?
	jz	short L1	; Jump if not

	int	41h		; Request WKD service

	jmp	short L2	; Join common code

L1:
	pushfd			; Pass flags
	cli			; Simulate INT environment
	push	cs		; Pass code selector
	call	near ptr LCL_INT41 ; Call the local handler
L2:
	endm			; INT41


; Common macros for 386SWAT

SETDATA macro	SREG,FL

ifnb <FL>
	pushf			; Save flags
endif				; IFNB <FL>
	push	cs		; Get code selector
	add	[esp].ELO,size DESC_STR ; Skip to data selector
	pop	SREG		; Address it
ifnb <FL>
	popf			; Restore flags
endif				; IFNB <FL>

	endm			; SETDATA


SETDATAX macro	SREG,FL

ifnb <FL>
	pushf			; Save flags
endif				; IFNB <FL>
	push	eax		; Save for a moment
	mov	eax,cs		; Get code selector
	add	eax,size DESC_STR ; Skip to data selector
	mov	SREG,eax	; Address it
	pop	eax		; Restore
ifnb <FL>
	popf			; Restore flags
endif				; IFNB <FL>
	endm			; SETDATAX


IGTMAC	macro	N1,N2,SRC,SAV
	local	L1,L2

	test	VCP_FLAG,@VCP_PMG ; Doing PM Interrupt Gates?
	jz	short L1	; Jump if not

%	IDTMAC_COM  PMG,N1,N2,SRC,SAV

	jmp	short L2	; Join common exit code


L1:
%	IDTMAC_COM  INT,N1,N2,SRC,SAV
L2:
	endm			; IGTMAC


IDTMAC	macro	N1,N2,SRC,SAV

%	IDTMAC_COM  INT,N1,N2,SRC,SAV

	endm			; IDTMAC


IDTMAC_COM macro INT,N1,N2,SRC,SAV

IDT	equ	<AGROUP:[ebx+N1*(type IDT_STR)]>

	mov	ax,SRC&INT&N2&_FVEC.FSEL ; Get selector
ifnb <SAV>
	xchg	ax,IDT.IDT_SELECT ; Swap with IDT
	mov	SAV&INT&N2&_FVEC.FSEL,ax ; Save for later use
else
	mov	IDT.IDT_SELECT,ax ; Save in IDT
endif

	mov	eax,SRC&INT&N2&_FVEC.FOFF ; Get 32-bit offset
ifnb <SAV>
	xchg	ax,IDT.IDT_OFFLO ; Swap with IDT
	mov	SAV&INT&N2&_FVEC.FOFF.ELO,ax ; Save for later use
else
	mov	IDT.IDT_OFFLO,ax ; Save in IDT
endif
	shr	eax,16		; Shift to low-order
ifnb <SAV>
	xchg	ax,IDT.IDT_OFFHI ; Swap with IDT
	mov	SAV&INT&N2&_FVEC.FOFF.EHI,ax ; Save for later use
else
	mov	IDT.IDT_OFFHI,ax ; Save in IDT
endif

	mov	al,SRC&INT&N2&_ARB ; Get incoming A/R byte
% ifdif <N2>,<01>
% ifdif <N2>,<03>
% ifdif <N2>,<41>
% ifdif <N2>,<68>
	and	al,not (mask $DT_DPL) ; Clear the DPL
	mov	ah,IDT.IDT_ACCESS ; Get current A/R byte
	and	ah,mask $DT_DPL ; Isolate the DPL
	or	al,ah		; Copy to incoming A/R byte
endif				; IFDIF <N2>,<68>
endif				; IFDIF <N2>,<41>
endif				; IFDIF <N2>,<03>
endif				; IFDIF <N2>,<01>
ifnb <SAV>
	xchg	al,IDT.IDT_ACCESS ; Swap with IDT
	mov	SAV&INT&N2&_ARB,al ; Save for later use
else
	mov	IDT.IDT_ACCESS,al ; Save in IDT
endif
	endm			; IDTMAC_COM


SELFBREAK macro lblname,flag,altjmp
	local	L1
	test	SELFDBG,flag	; Are we debugging ourselves?
ifnb <altjmp>
	jz	altjmp		;
else
	jz	short L1	; If so, then skip this section
endif
	and	SELFDBG,not (flag) ; Clear self debug flag so we don't get
				; a breakpoint on top of a breakpoint...
	public	lblname&_BREAKPT
lblname&_BREAKPT:
	int	1		; Trigger breakpoint
L1:
	endm			; SELFBREAK


; Keystroke macro

KSTMAC	macro	KEY,ACT


KEYSEG	segment use32 word public 'data' ; Start KEYSEG segment
	assume	ds:DGROUP

	dw	@KEY_&KEY	; Define the keycode

KEYSEG	ends			; End KEYSEG segment


ACTSEG	segment use32 dword public 'data' ; Start ACTSEG segment
	assume	ds:DGROUP

	dd	offset PGROUP:ACT ; Define the corresponding action

ACTSEG	ends			; End ACTSEG segment

	endm			; End KSTMAC macro


; Structure for TOGINTxx tables

INTR_STR struc

INTR_IMSK dd	?		; 00:  Interrupt # mask
INTR_NUMB dd	?		; 04:  Interrupt #
INTR_DOFF dd	?		; 08:  Offset in DGROUP of flag word if LOD SWAT
INTR_DMSK dd	?		; 0C:  Mask if LOD SWAT (in INTR_DOFF)
INTR_RMSK dd	?		; 10:  Mask if device SWAT (in TRP_FLAG)
INTR_PLCL_FVEC dd ?		; 14:  Offset in DGROUP of LCLINTxx_FVEC
INTR_PLCL_ARB  dd ?		; 18:  ...		   LCLINTxx_ARB
INTR_POLD_FVEC dd ?		; 1C:  ...		   OLDINTxx_FVEC
INTR_POLD_ARB  dd ?		; 20:  ...		   OLDINTxx_ARB
INTR_PTSS_FVEC dd ?		; 24:  ...		   TSSINTxx_FVEC
INTR_PTSS_ARB  dd ?		; 28:  ...		   TSSINTxx_ARB
INTR_POLDTSS_FVEC dd ?		; 2C:  ...		   OLDTSSINTxx_FVEC
INTR_POLDTSS_ARB  dd ?		; 30:  ...		   OLDTSSINTxx_ARB
INTR_ON   dd	?		; 34:  ...		   ON message
INTR_OFF  dd	?		; 38:  ...		   OFF ...
INTR_PLCG_FVEC dd ?		; 3C:  ...		   LCLPMGxx_FVEC
INTR_PLCG_ARB  dd ?		; 40:  ...		   LCLPMGxx_ARB
INTR_POLG_FVEC dd ?		; 44:  ...		   OLDPMGxx_FVEC
INTR_POLG_ARB  dd ?		; 48:  ...		   OLDPMGxx_ARB

INTR_STR ends

