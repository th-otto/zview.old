;   $Id: stdcall.s 82 2012-03-18 15:49:44Z landemarre $
;
; This set of function allows a process (compiled against a C compiler)
; to "branch" to a subroutine compiled against another C compiler.
; 
; This kind of "branchement" is used by LDG library, or TSR, or...
;
; The problem: 
; 
; the list of scratch registers differs from a C compiler to another one.
; Scratch register are register that may be altered by a CDECL function.
;
; for GCC,     D0-D1 and A0-A1 are the scratch registers
; for Pure C,  D0-D2 and A0-A1 are the scratch registers
; for Sozobon, D0-D2 and A0-A2 are the scratch registers
;
; For example, if an application compiled with GCC invoke a CDECL function
; compiled with PureC.
; On "code compiled with gcc" side, D2 register is used to store some
; internal data, then the code "branch" to the CDECL function (which may
; not alter D2 register), and when the CDECL function returns, the gcc
; continue to work but one of its internal data (previously stored in D2)
; may has been changed by the CDECL function because D2 is a scratch register
; for Pure C.
; 
; The solution:
;
; save and restore D2 and A2 register when such branchement may occurs.
;
; Implementation:
;
; All functions of a LDG library are redirected to a function that will
; - save D2/A2
; - call the original CDECL function of the library
; - restore D2/A2
; This way, from the "client" point of view (the client is the application
; which call a function of a LDG library), A2 and D2 registers are preserved.
; This redirection is not needed for libraries compiled with GCC (because GCC
; already preserve D2 and A2 registers)
;
; => this is TRNFM_TO_STDCALL
;
; When a piece of code have to invoke callback function (for example a function
; of a LDG library invoke a callback function of one of its client), the function
; ldg_callback() should be used. This function will do the same job.
;
; Copyright (c) 2001,2005 Arnaud BERCEGEAY <bercegeay@atari.org>
; Patch Coldfire Vicent Riviere
; 

; preprocessor (sed) job:
; purec does not set an underscore character in front of symbols defined
; in the C side... whereas all other compilers do.
; --> use  in this code. This string will be replaced by nothing
; for purec version of this file, and by an underscore for all other
; compilers.


; ***************************************************************
;   Global data 
; ***************************************************************

; data declared in the C part. 
; array of "structures" where A2/D2 registers are stored

	.globl _ldg_adr_regctx, _ldg_nb_regctx

; section DATA
	.data

; semaphore (mutex) to protect the writing access to _ldg_adr_regctx
; and _ldg_nb_regctx. Set to 0 means "unused" or free to be used. Set
; to 1 means locked or "in-used please wait"

verrou:
	.dc.b	0

	
; section TEXT
	.text


; ***************************************************************
;   TRNFM_TO_STDCALL
; ***************************************************************
;
; in the C part of this library, each function of the library is
; replaced by the following piece of code:
; LEA #address_of_the_original_CDECL_function, A0
; JMP _ldg_begin_stdcall
;

; the entry point of TRNFM_TO_STDCALL:
; here A0 contains the original address of the CDECL function,
; so A0 should not be overwritten.

	.globl	_ldg_begin_stdcall

_ldg_begin_stdcall:

; wait for availability of the "regctx" data in writing mode
; by using the "verrou" semaphore

lock:
	moveq	#0,d0
	bset	d0, verrou
	bne	lock

;  parse the list of ctxreg and look for the 1st free ctxreg data
;  a "free" ctxreg data
;  a ctxreg is considered as free is the 1st long word of this 
;  structure is 0L.
;  If no free ctxreg structure is available, then D2/A2 registers
;  are not saved. The original CDECL function is invoking...
;  (what else can we do ? send an TERM sig ?)
	
	move.l	#_ldg_adr_regctx,a1
	move.l	_ldg_nb_regctx,d0
tst_ctx:
	tst.l		d0
	beq		go_to_real_function
	tst.l		(a1)
	beq		save_ctx
	subq.l	#1,d0
	adda.l	#16,a1
	bra		tst_ctx

;  A free ctxreg structure has been found (address in A1)
;  save the context (d2,a2,a3,(a7)) :
;  - D2 and A2 because they have to be saved ;)
;  - (a7) contain the return address (because we'll change it)
;  - a3 because we'll use this register to save "this" (address of
;    the regctx data)

save_ctx:
	adda.l	#16,a1
	lea	-12(a1),a1
	movem.l	d2/a2/a3,(a1)
	move.l	(a7),-(a1)
	move.l	a1,a3

;  change the return address so that __ldg_end_stdcall is invoked
;  when the original CDECL function will return.

	move.l	#__ldg_end_stdcall,(a7)
	
go_to_real_function:

;  free the mutex and call the original CDECL function (address in A0
;  since the beginning)

	clr.b	verrou
	jmp		(a0)

;  When the original CDECL function returns, we have to restore the 
;  contexte (address of the context stored in A3)
;  WARNING: D0 is the return value of the original CDECL function and
;  shall not be changed !

__ldg_end_stdcall:

;  save address of ctxreg in A1 (because A3 will be overwritten
;  when registers will be restored)
	
	move.l	a3,a1
	move.l	(a1)+,a0
	movem.l	(a1),d2/a2/a3
	lea	12(a1),a1

;  Set to 0L the first long word of the regctx to free it.
;  remark: no need to use the mutex to do such operation.

	clr.l		-16(a1)

;  back to the caller

	jmp		(a0)
	

; ***************************************************************
;   ldg_callback( void *f, ...)
;
;   This function will
;   - save A2/D2 registers
;   - call the "f" functions with the "..." paramters
;   - restore A2/D2 registers
; ***************************************************************
;

	.globl	ldg_callback

ldg_callback:

;  value in the stack are :
;  (a7)   : address to go back in RTS
;  4(a7) : f
;  x(a7) : parameters of the function "f"

; wait for availability of the "regctx" data in writing mode
; by using the "verrou" semaphore

lock2:
	moveq	#0,d0
	bset	d0, verrou
	bne	lock2

;  parse the list of ctxreg and look for the 1st free ctxreg data
;  a "free" ctxreg data
;  a ctxreg is considered as free is the 1st long word of this 
;  structure is 0L.
	
	move.l	#_ldg_adr_regctx,a1
	move.l	_ldg_nb_regctx,d0
tst_ctx2:
	tst.l		d0
	beq		abort_callback
	tst.l		(a1)
	beq		save_ctx2
	subq.l	#1,d0
	adda.l	#16,a1
	bra		tst_ctx2

;  A free ctxreg structure has been found (address in A1)
;  save the context (d2,a2,a3,(a7)) :
;  - D2 and A2 because they have to be saved ;)
;  - (a7) contain the return address (because we'll change it)
;  - a3 because we'll use this register to save "this" (address of
;    the regctx data)

save_ctx2:
	adda.l	#16,a1
	lea	-12(a1),a1
	movem.l	d2/a2/a3,(a1)
	move.l	(a7),-(a1)
	move.l	a1,a3

;  free the mutex now

	clr.b	verrou
	
;  save the address of "f" in A0

	move.l 4(a7),a0

;  move the stack pointer so that paramters of "f" are well positionned
;  in relation to A7. The consequence is we'll lose the content of (A7)
;  but it's not a pb because we've already saved it in regctx.

	adda.l  #4,a7
 
;  change the return address so that "end_callback" is invoked
;  when the original CDECL function will return.

	move.l	#end_callback,(a7)
	
;  call the "f" function (address in A0)

	jmp		(a0)

;  When the original CDECL function returns, we have to restore the 
;  contexte (address of the context stored in A3)
;  WARNING: D0 is the return value of the original CDECL function and
;  shall not be changed !

end_callback:

;  save address of ctxreg in A1 (because A3 will be overwritten
;  when registers will be restored)
	
	move.l	a3,a1
	move.l	(a1)+,a0
	movem.l	(a1),d2/a2/a3
	lea	12(a1),a1

;  Set to 0L the first long word of the regctx to free it.
;  remark: no need to use the mutex to do such operation.

	clr.l		-16(a1)

;  restore the original stack position and go back to the caller

	suba.l #4,a7
	move.l  a0,-(a7)
	rts

;  when context cannot be saved in regctx, the best to do is to leave now!

abort_callback:
	rts
