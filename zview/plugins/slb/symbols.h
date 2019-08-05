/*
 * symbols.h - internal header file.
 * Add (or not) underscores to external symbol names
 *
 * Copyright (C) 2018 Thorsten Otto
 *
 * For conditions of distribution and use, see copyright notice in zlib.h
 */
#ifndef __USER_LABEL_PREFIX__
#  if defined(__ELF__)
#    define __USER_LABEL_PREFIX__
#  else
#    define __USER_LABEL_PREFIX__ _
#  endif
#endif

#define __STRING(x)	#x
#define __STRINGIFY(x)	__STRING(x)

#ifndef __SYMBOL_PREFIX
# define __SYMBOL_PREFIX __STRINGIFY(__USER_LABEL_PREFIX__)
# define __ASM_SYMBOL_PREFIX __USER_LABEL_PREFIX__
#endif

#ifndef C_SYMBOL_NAME
# ifdef __ASSEMBLER__
#   define C_SYMBOL_NAME2(pref, name) pref##name
#   define C_SYMBOL_NAME1(pref, name) C_SYMBOL_NAME2(pref, name)
#   define C_SYMBOL_NAME(name) C_SYMBOL_NAME1(__ASM_SYMBOL_PREFIX, name)
# else
#   define C_SYMBOL_NAME(name) __SYMBOL_PREFIX #name
# endif
#endif


#undef SLB_NWORDS
#define SLB_NWORDS(_nargs) ((((long)(_nargs) * 2 + 1l) << 16) | (long)(_nargs))
#undef SLB_NARGS
#define SLB_NARGS(_nargs) SLB_NWORDS(_nargs)

#define LIBFUNC(name, get) \
static void __attribute__((used)) __ ## name ## _entry(void) \
{ \
	register SLB *lib __asm__("a0"); \
 \
	__asm__ __volatile__ ( \
		" .globl " C_SYMBOL_NAME(name) "\n" \
C_SYMBOL_NAME(name) ":\n"); \
 \
	lib = get(); \
 \
	__asm__ __volatile__ ( \
		" move.l (%%a7)+,%%d0\n" /* get return pc */ \
		" move.l %[nargs],-(%%a7)\n"  /* push number of args */ \
		" clr.l -(%%a7)\n" /* push function number */ \
		" move.l (%[lib]),-(%%a7)\n" /* push SLB handle */ \
		" move.l %%d0,-(%%a7)\n" /* push return pc */ \
		" move.l 4(%[lib]),%[lib]\n" /* get exec function */ \
		" jmp (%[lib])\n"          /* go for it */ \
	: \
	: [lib]"r"(lib), [nargs]"i"(SLB_NARGS(2)) \
	: "cc", "memory" \
	); \
	__builtin_unreachable(); \
}
