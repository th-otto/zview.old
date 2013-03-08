	.text

	.globl	_plane2packed24

| int32	plane2packed24( int32 no_words, int32 plane_length, int16 no_planes, void *src, void *dst, CTAB_REF src_ctab );
|                       d0              d1                  d2               a0         a1         4(sp)

| 11x4 + 4 + 5x4

_plane2packed24:
	movem.l		d2-d7/a2-a6,-(sp)

	move.l		11*4+4(sp),d0
	move.l		11*4+8(sp),d1
	move.l		11*4+12(sp),d2
	movea.l		11*4+16(sp),a0
	movea.l		11*4+20(sp),a1
	
	movea.l		11*4+24(sp),a6						|source of the color table

	move.l		d0,d4
	subq.l		#1,d4
	move.l		d1,d6
	add.l		d6,d6
	add.l		d6,d6

	movea.l		a1,a4									|destination's adress to A4
	lea			(a0,d1.l),a1
	lea			(a1,d1.l),a2
	lea			(a2,d1.l),a3

	lea			not_supported(pc),a5

	cmpi.w		#8,d2
	bhi.s		plane2packed24_transform

	lea			plane2packed24_table(pc),a5
	add.w		d2,d2
	adda.w		0(a5,d2.w),a5

plane2packed24_transform:
	jsr			(a5)

	movem.l		(sp)+,d2-d7/a2-a6
	rts

plane2packed24_table:
	DC.w		not_supported-plane2packed24_table
	DC.w		pp_1_to_8-plane2packed24_table						|1 Bit
	DC.w		not_supported-plane2packed24_table
	DC.w		not_supported-plane2packed24_table
	DC.w		pp_4_to_24-plane2packed24_table						|4 Bit
	DC.w		not_supported-plane2packed24_table
	DC.w		not_supported-plane2packed24_table
	DC.w		not_supported-plane2packed24_table
	DC.w		pp_8_to_24-plane2packed24_table						|8 Bit


not_supported:
	moveq		#0,d0
	rts

pp_1_to_8:
	move.w		(a0)+,d0
	moveq		#15,d5

pp_1_to_8_exp:
	moveq		#0,d6

	add.w		d0,d0
	addx.l		d6,d6

	move.b		d6,(a4)+

	dbra		d5,pp_1_to_8_exp

	subq.l		#1,d4
	bpl.b		pp_1_to_8
	moveq		#1,d0
	rts


pp_4_to_24:				move.w		(a0)+,d0
						move.w		(a1)+,d1
						move.w		(a2)+,d2
						move.w		(a3)+,d3

						moveq		#15,d5
pp_4_to_24_exp:			moveq		#0,d6
						add.w		d3,d3
						addx.w		d6,d6
						add.w		d2,d2
						addx.w		d6,d6
						add.w		d1,d1
						addx.w		d6,d6
						add.w		d0,d0
						addx.w		d6,d6


						lsl.w		#2,d6

						lea			(a6,d6.w),a5
						move.b		(a5),(a4)+						|R
						move.b		1(a5),(a4)+						|G
						move.b		2(a5),(a4)+						|B

pp_4_to_24_next:		dbra		d5,pp_4_to_24_exp

						subq.l		#1,d4
						bpl.b		pp_4_to_24
						moveq		#1,d0
						rts


pp_8_to_24:				move.l		d4,-(sp)
						move.l		d6,-(sp)

						move.w		(a0)+,d0
						move.w		(a1)+,d1
						move.w		(a2)+,d2
						move.w		(a3)+,d3
						move.w		-2(a0,d6.l),d4
						move.w		-2(a1,d6.l),d5
						move.w		-2(a3,d6.l),d7
						move.w		-2(a2,d6.l),d6

						swap		d0
						swap		d5
						move.w		#15,d5
pp_8_to_24_exp:			swap		d5
						clr.w		d0
						add.w		d7,d7
						addx.w		d0,d0
						add.w		d6,d6
						addx.w		d0,d0
						add.w		d5,d5
						addx.w		d0,d0
						add.w		d4,d4
						addx.w		d0,d0
						add.w		d3,d3
						addx.w		d0,d0
						add.w		d2,d2
						addx.w		d0,d0
						add.w		d1,d1
						addx.w		d0,d0
						swap		d0
						add.w		d0,d0
						swap		d0
						addx.w		d0,d0

						lsl.w		#2,d0

						lea			(a6,d0.w),a5
						move.b		(a5),(a4)+						|R
						move.b		1(a5),(a4)+						|G
						move.b		2(a5),(a4)+						|B

						swap		d5
						dbra		d5,pp_8_to_24_exp

						move.l		(sp)+,d6
						move.l		(sp)+,d4
						subq.l		#1,d4
						bpl.b		pp_8_to_24
						moveq		#1,d0
						rts
