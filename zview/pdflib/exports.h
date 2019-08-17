/*
 * zvpdf/exports.h - internal header with definitions of all exported functions
 *
 * Copyright (C) 2019 Thorsten Otto
 *
 * For conditions of distribution and use, see copyright file.
 */

#ifndef LIBFUNC
# error "LIBFUNC must be defined before including this file"
#endif
#ifndef LIBFUNC2
#define LIBFUNC2(_fn, name, _nargs) LIBFUNC(_fn, name, _nargs)
#endif

/*   0 */ LIBFUNC(0, slb_control, 2)
/*   1 */ LIBFUNC(1, pdf_init, 1)
/*   2 */ LIBFUNC(2, pdf_exit, 0)
/*   3 */ LIBFUNC(3, lib_pdf_load, 3)
/*   4 */ LIBFUNC(4, pdf_get_page_size, 4)
/*   5 */ LIBFUNC(5, pdf_decode_page, 4)
/*   6 */ LIBFUNC(6, pdf_get_page_addr, 1)
/*   7 */ LIBFUNC(7, pdf_quit, 1)
/*   8 */ LIBFUNC(8, get_pdf_title, 0)
/*   9 */ LIBFUNC(9, pdf_build_bookmark, 2)
/*  10 */ LIBFUNC(10, pdf_get_info, 2)
/*  11 */ LIBFUNC(11, delete_bookmarks, 1)

#undef LIBFUNC
#undef LIBFUNC2
#undef NOFUNC
