#include "asmdefs.h"

/*
 * Signum! image decompressoion code
 * code taken from Imagic demo -> denisdem.prg
 * size of unpacked data is always 32000 bytes (st high resolution)
 * dis-assembled and modified by Lonny Pursell
 */

/*
 * note: destination buffer must be all 0's -> memset(dst, 0, 32000);
 *      decompressor uses some kind of skip code, so ram cannot be dirty
 *      after checking the 8 byte file id, call this subroutine
 *      thus source address would be start of file + 8
 */

/* extern cdecl depack_imc(void *src, void *dst); */

SRC_ADR	= (15*4)+4
DST_ADR	= (15*4)+8

	.globl	C_SYMBOL_NAME(depack_imc)

	.text

C_SYMBOL_NAME(depack_imc):
#ifdef __mcoldfire__
	lea     -(15*4)(sp),sp
    movem.l	d0-d7/a0-a6,(sp)
#else
    movem.l	d0-d7/a0-a6,-(sp)
#endif
	movea.l	SRC_ADR(sp),a0
	movea.l	DST_ADR(sp),a1
	moveq	#80,d7
	movea.l	a1,a2
	link	a4,#-46
	moveq	#0,d5
	move.w	8(a0),-2(a4)
	move.w	10(a0),-4(a4)
	move.w	20(a0),-6(a4)
	movea.l	a0,a6
	lea 	32(a6),a6
	moveq	#8,d6
	movea.l	a6,a5
	adda.l	12(a0),a5
	move.l	a2,-10(a4)
	move.w	d7,d4
	ext.l	d4
	asl.l	#4,d4
#ifdef __mcoldfire__
	moveq	#0,d3
#endif
	move.w	-4(a4),d3
	bra	depack_imc3

#ifdef __mcoldfire__
#define BCC bne.s
#else
#define BCC bcc.s
#endif

depack_imc1:
	bsr	getbit
	BCC	depack_imc2
	bsr	L2AE82
depack_imc2:
	add.l	d4,-10(a4)
depack_imc3:
#ifdef __mcoldfire__
	subq.l #1,d3
	bpl.s depack_imc1
#else
	dbf	d3,depack_imc1
#endif
	bsr	L2AFC4
	unlk	a4
#ifdef __mcoldfire__
	movem.l	(sp),d0-d7/a0-a6
	lea     (15*4)(sp),sp
#else
	movem.l	(sp)+,d0-d7/a0-a6
#endif
	rts

L2AE82:
	move.l	d3,-(a7)
	move.l	-10(a4),-14(a4)
#ifdef __mcoldfire__
	moveq	#0,d3
#endif
	move.w	-2(a4),d3
	bra	L2AEA0

L2AE92:
	bsr	getbit
	BCC	L2AE9C
	bsr	L2AEA8
L2AE9C:
	addq.l	#2,-14(a4)
L2AEA0:
#ifdef __mcoldfire__
	subq.l #1,d3
	bpl.s L2AE92
#else
	dbf	d3,L2AE92
#endif
	move.l	(a7)+,d3
	rts

#ifdef __mcoldfire__
#define ADDQW addq.l
#else
#define ADDQW addq.w
#endif

L2AEA8:
	move.l	a3,-(a7)
	move.l	d3,-(a7)
	moveq	#0,d3
	bsr	getbit
	BCC	L2AEB6
	ADDQW	#2,d3
L2AEB6:	bsr	getbit
	BCC	L2AEBE
	ADDQW	#1,d3
L2AEBE:
	cmpi.w	#3,d3
	beq	L2AF20
	bsr	L2AFA2
	lea 	-46(a4),a3
	bsr	getbit
	BCC	L2AEDA
	movea.l	a3,a0
	bsr	L2AF42
L2AEDA:	addq.l	#1,a3
	bsr	getbit
	BCC	L2AEE8
	movea.l	a3,a0
	bsr	L2AF42
L2AEE8:	lea 	15(a3),a3
	bsr	getbit
	BCC	L2AEF8
	movea.l	a3,a0
	bsr	L2AF42
L2AEF8:	addq.l	#1,a3
	bsr	getbit
	BCC	L2AF06
	movea.l	a3,a0
	bsr	L2AF42
L2AF06:	cmpi.w	#1,d3
	beq.s	L2AF14
	cmpi.w	#2,d3
	beq.s	L2AF1A
	bra.s	L2AF24

L2AF14:	bsr	L2AF56
	bra.s	L2AF24

L2AF1A:	bsr	L2AF6A
	bra.s	L2AF24

L2AF20:	bsr	L2AF2E
L2AF24:	bsr	L2AF7E
	move.l	(a7)+,d3
	move.l	(a7)+,a3
	rts

L2AF2E:
	lea 	-46(a4),a0
	moveq	#7,d0
L2AF34:
	move.b	(a5)+,(a0)+
	move.b	(a5)+,(a0)+
	move.b	(a5)+,(a0)+
	move.b	(a5)+,(a0)+
#ifdef __mcoldfire__
	subq.l #1,d0
	bpl.s L2AF34
#else
	dbf	d0,L2AF34
#endif
	rts

L2AF42:
	move.b	(a5)+,d1
	beq.s	L2AF54
	moveq	#7,d0
L2AF48:
#ifdef __mcoldfire__
	add.l	d1,d1
	btst    #8,d1
#else
	add.b	d1,d1
#endif
	BCC	L2AF4E
	move.b	(a5)+,(a0)
L2AF4E:
	addq.l	#2,a0
#ifdef __mcoldfire__
	subq.l #1,d0
	bpl.s L2AF48
#else
	dbf	d0,L2AF48
#endif
L2AF54:
	rts

L2AF56:
	lea 	-46(a4),a0
	move.w	(a0)+,d0
	moveq	#14,d2
L2AF5E:
	move.w	(a0),d1
#ifdef __mcoldfire__
	eor.l	d1,d0
#else
	eor.w	d1,d0
#endif
	move.w	d0,(a0)+
#ifdef __mcoldfire__
	subq.l #1,d2
	bpl.s L2AF5E
#else
	dbf	d2,L2AF5E
#endif
	rts

L2AF6A:
	lea 	-46(a4),a0
	move.l	(a0)+,d0
	moveq	#6,d2
L2AF72:
	move.l	(a0),d1
	eor.l	d1,d0
	move.l	d0,(a0)+
#ifdef __mcoldfire__
	subq.l #1,d2
	bpl.s L2AF72
#else
	dbf	d2,L2AF72
#endif
	rts

L2AF7E:
	lea 	-46(a4),a0
	movea.l	-14(a4),a1
	move.w	d7,d1
	ext.l	d1
	moveq	#3,d0
L2AF8C:
	move.w	(a0)+,(a1)
	adda.l	d1,a1
	move.w	(a0)+,(a1)
	adda.l	d1,a1
	move.w	(a0)+,(a1)
	adda.l	d1,a1
	move.w	(a0)+,(a1)
	adda.l	d1,a1
#ifdef __mcoldfire__
	subq.l #1,d0
	bpl.s L2AF8C
#else
	dbf	d0,L2AF8C
#endif
	rts

L2AFA2:
	lea 	-46(a4),a0
	clr.l	(a0)+
	clr.l	(a0)+
	clr.l	(a0)+
	clr.l	(a0)+
	clr.l	(a0)+
	clr.l	(a0)+
	clr.l	(a0)+
	clr.l	(a0)+
	rts

getbit:
#ifdef __mcoldfire__
	subq.l #1,d5
	bpl.s getbit1
#else
	dbf	d5,getbit1
#endif
	moveq	#7,d5
#ifdef __mcoldfire__
	moveq	#0,d6
#endif
	move.b	(a6)+,d6
getbit1:
#ifdef __mcoldfire__
	add.l	d6,d6
	btst    #8,d6
getbit2:
#else
	add.b	d6,d6
#endif
	rts

L2AFC4:
	tst.w	-6(a4)
	bne.s	L2AFCC
	rts

L2AFCC:
	movea.l	a2,a0
	clr.l	d0
	move.b	-6(a4),d0
	bsr	L2AFE8
	movea.l	a2,a0
#ifdef __mcoldfire__
	adda.l	d7,a0
#else
	adda.w	d7,a0
#endif
	clr.l	d0
	move.b	-5(a4),d0
	bsr	L2AFE8
	rts

L2AFE8:
#ifdef __mcoldfire__
	move.l	d0,d1
	lsl.l	#8,d1
	or.l	d1,d0
	moveq #0,d1
	move.w	-4(a4),d1
	asl.l	#3,d1
#else
	move.w	d0,d1
	lsl.w	#8,d1
	or.w	d1,d0
	move.w	-4(a4),d1
	asl.w	#3,d1
#endif
	bra	L2B00C

L2AFF8:
	movea.l	a0,a1
#ifdef __mcoldfire__
	adda.l	d7,a0
	adda.l	d7,a0
	moveq #0,d2
#else
	adda.w	d7,a0
	adda.w	d7,a0
#endif
	move.w	-2(a4),d2

#ifdef __mcoldfire__
	move.l d3,-(a7)
	bra	L2B008
L2B006:
	move.w (a1),d3
	eor.l	d0,d3
	move.w d3,(a1)+
L2B008:
	subq.l #1,d2
	bpl.s L2B006
L2B00C:
	subq.l #1,d1
	bpl.s L2AFF8
	move.l (a7)+,d3
	rts

#else

	bra	L2B008
L2B006:
	eor.w	d0,(a1)+
L2B008:
	dbf	d2,L2B006
L2B00C:
	dbf	d1,L2AFF8
	rts

#endif
