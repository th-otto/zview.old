/*
 * symbols.h - internal header file.
 * Add (or not) underscores to external symbol names
 *
 * Copyright (C) 2018 Thorsten Otto
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
    /* avoid empty macro arguments */
#   define _ 2
#   if (__ASM_SYMBOL_PREFIX + 0) > 1
#     define C_SYMBOL_NAME(name) C_SYMBOL_NAME1(__ASM_SYMBOL_PREFIX, name)
#   else
#     define C_SYMBOL_NAME(name) name
#   endif
#   undef _
# else
#   define C_SYMBOL_NAME(name) __SYMBOL_PREFIX #name
# endif
#endif
