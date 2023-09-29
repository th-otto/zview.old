dnl configure.ac, used by autoconf
dnl to create the configure script
dnl @(#) Thorsten Otto, 26.04.2019
dnl
AC_PREREQ([2.69])

AC_INIT([xpdf],[4.04],[https://www.xpdfreader.com/],[],[https://www.xpdfreader.com/])
AC_CONFIG_SRCDIR(xpdf/Gfx.cc)

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR(build-aux)

###############################################################################
#
#       Autoheader stuff
#
###############################################################################

AH_TOP([
#ifndef ACONF_H
#define ACONF_H

])

AH_BOTTOM([
/*
 * This gets included by aconf.h, and contains miscellaneous global
 * settings not directly controlled by autoconf.  This is a separate
 * file because otherwise the configure script will munge any
 * #define/#undef constructs.
 *
 * Copyright 2002-2003 Glyph & Cog, LLC
 */

#if defined(__cplusplus) && defined(USE_EXCEPTIONS)
# if __cplusplus >= 201103L
#  define NO_THROWS noexcept(true)
#  define THROWS(a, ...) noexcept(false)
# else
#  define NO_THROWS throw()
#  define THROWS(a, ...) throw(a ## __VA_ARGS__)
# endif
#else
# define NO_THROWS
# define THROWS(a, ...)
#endif

#endif /* ACONF_H */
])

AC_CANONICAL_HOST

if test "$ECHO_E" = ""; then
  ECHO_E="echo -e"
fi
AC_SUBST(ECHO_E)

AC_CONFIG_HEADERS([aconf.h])

AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE([enable])

m4_define([AC_ADD_CFLAG], [
for flag in $2; do
case " [$]$1 " in
"* $flag *") ;;
*) AC_MSG_CHECKING([whether the compiler understands] $flag)
   saved_$1="[$]$1"
   $1="[$]$1 $flag"
   AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]], [[]])],[result=yes],[$1="$saved_$1"; result=no])
   AC_MSG_RESULT([$result])
   ;;
esac
done
])

###############################################################################
#
#       Shortcut for AC_ARG_ENABLE
#
###############################################################################

m4_define(ac_option_ucase, [translit($1,-abcdefghijklmnopqrstuvwxyz,_ABCDEFGHIJKLMNOPQRSTUVWXYZ)])dnl
m4_define(ac_option_lcase, [translit($1,-ABCDEFGHIJKLMNOPQRSTUVWXYZ,_abcdefghijklmnopqrstuvwxyz)])dnl

# AH_OPTION(OPTION, DESC, DEFAULT, DEFINE)
# ----------------------------------------
AC_DEFUN([AC_OPTION], [
AH_TEMPLATE([ac_option_ucase(m4_case($4,,$1,$4))], [$2])
AC_ARG_ENABLE($1,
[AS_HELP_STRING([--m4_case($3,,enable,no,enable,yes,disable,must-be-yes-or-no)-$1],[$2])],
[case [$]enableval in
yes|no)
     enable_[]ac_option_lcase($1)=[$]enableval
     ;;
*)   AC_MSG_ERROR([must be yes or no: --enable-[]$1=[$]enableval]) ;;
esac
], [enable_[]ac_option_lcase($1)=m4_case($3,,no,$3)])
if test "[$]enable_[]ac_option_lcase($1)" = "yes"; then
     AC_DEFINE(ac_option_ucase(m4_case($4,,$1,$4)))
fi
])

###############################################################################
###############################################################################
#
#       End of function definitions.  Now start actually executing stuff.
#
###############################################################################
###############################################################################

# stuff for Makefiles
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AC_CHECK_TOOL(STRIP, strip, :)

# Support silent build rules, requires at least automake-1.11. Disable
# by either passing --disable-silent-rules to configure or passing V=1
# to make
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AC_PROG_CC
AC_EXEEXT
AC_C_CONST
AC_PROG_CC_C_O
AX_PROG_CC_FOR_BUILD
PKG_PROG_PKG_CONFIG

###############################################################################
#
# Early compiler setup.
#
###############################################################################

TARGET_OS=unix
case $host in
  *-*-mingw*|*-*-msys*|*-*-MINGW*|*-*-MSYS*|*-*-cygwin*|*-*-CYGWIN*)
     test "$CC" = "" && CC=gcc
     test "$CXX" = "" && CXX="g++"
     for arg in -mms-bitfields -mno-cygwin -mwin32 -mthreads; do
        case $CC in
        *$arg*) ;;
        *) if $CC $arg -E - < /dev/null >/dev/null 2>&1; then
             echo adding $arg to $CC
             CC="$CC $arg"
             CXX="$CXX $arg"
           fi
        esac
     done
     TARGET_OS=win32
     LIBS="-lole32"
     case $host in
  	x86_64-*-*) RC_TARGET=pe-x86-64 ;;
	*) RC_TARGET=pe-i386 ;;
     esac
     ;;
  *-apple-darwin* )
     test "$CC" = "" && CC=gcc
     test "$CXX" = "" && CXX="g++"
     for arg in -fno-writable-strings -mmacosx-version-min=10.6; do
        case $CC in
        *$arg*) ;;
        *) if $CC $arg -E - < /dev/null >/dev/null 2>&1; then
             echo adding $arg to $CC
             CC="$CC $arg"
             CXX="$CXX $arg"
           fi
        esac
     done
     ECHO_E="echo"
     CFLAGS="$CFLAGS $CPROJECTFLAGS"
     LDFLAGS="$LDFLAGS $LDPROJECTFLAGS"
     TARGET_OS=mac
     ;;
  m68k*-*-mint* | *-atarist-* )
     TARGET_OS=tos
     ;;
  *)
     ;;
esac
AC_SUBST(RC_TARGET)

if test "$cross_compiling" = no; then
  # do not consider this as a native build only because we can exec build programs;
  # it might just be a binfmt handler that makes this possible
  case $build in *-*-linux*)
     case $host in
     *-*-linux*) ;;
     *) cross_compiling=yes ;;
     esac
  esac
fi

AC_ARG_ENABLE(warnings,
[AS_HELP_STRING([--enable-warnings],[enable compiler warnings])],
[case $enableval in
yes)
     for arg in -Wall -W -Wstrict-prototypes -Wmissing-prototypes \
           -Wmissing-declarations -Wwrite-strings \
           -Wcast-qual -Wdeclaration-after-statement -Wundef -Wc++-compat -Wno-nonnull-compare
     do
         AC_ADD_CFLAG(CFLAGS, $arg)
     done
     for arg in -Wall -W -Wmissing-declarations -Wwrite-strings \
           -Wcast-qual -Wundef -Wno-nonnull-compare
     do
         AC_ADD_CFLAG(CXXFLAGS, $arg)
     done
     ;;
no)  ;;
*)   AC_MSG_ERROR([must be yes or no: --enable-warnings=$enableval]) ;;
esac
])

AC_ARG_ENABLE(fatal-warnings,
[AS_HELP_STRING([--enable-fatal-warnings],[Turn compiler warnings into errors [default=disabled]])],
[case $enableval in
yes) enable_fatal_warnings=yes
     ;;
no)  enable_fatal_warnings=no
     ;;
*)   AC_MSG_ERROR([must be yes or no: --enable-fatal-warnings=$enableval]) ;;
esac
], [enable_fatal_warnings=no])

AM_CONDITIONAL(OS_WIN32, test "$TARGET_OS" = "win32")
AM_CONDITIONAL(OS_TOS, test "$TARGET_OS" = "tos")
AM_CONDITIONAL(OS_UNIX, test "$TARGET_OS" = "unix")
AM_CONDITIONAL(OS_MACOS, test "$TARGET_OS" = "mac")
AM_CONDITIONAL(CROSS_COMPILING, test "$cross_compiling" != no)

dnl ##### Switch over to C++.  This will make the checks below a little
dnl ##### bit stricter (requiring function prototypes in include files).
dnl ##### (99% of xpdf is written in C++.)
AC_LANG([C++])

AC_CHECK_HEADERS([argz.h limits.h locale.h nl_types.h malloc.h string.h strings.h unistd.h process.h sys/param.h endian.h io.h fcntl.h dlfcn.h pwd.h wchar.h])

AC_CHECK_FUNCS([popen mktemp mkstemp mkstemps fseeko fseek64 _fseeki64])

AC_CHECK_LIB(m, log)

###############################################################################
# miscellaneous options
###############################################################################

AH_TEMPLATE([A4_PAPER], [use A4 (instead of Letter) paper size by default for PostScript output])
AC_OPTION(a4-paper, [use A4 (instead of Letter) paper size by default for PostScript output], no)
AH_TEMPLATE([NO_TEXT_SELECT], [do not allow text selection])
AC_OPTION(no-text-select, [do not allow text selection], no)
AH_TEMPLATE([OPI_SUPPORT], [include support for OPI comments])
AC_OPTION(opi, [include support for OPI comments], no, [OPI_SUPPORT])
AH_TEMPLATE([MULTITHREADED], [include support for multithreading])
AC_OPTION(multithreaded, [include support for multithreading], yes)
if test "$enable_multithreaded" = yes; then
	AX_PTHREAD
fi
AM_CONDITIONAL(MULTITHREADED, test "$enable_multithreaded" = yes)
AH_TEMPLATE([USE_EXCEPTIONS], [use C++ exceptions])
AC_OPTION(exceptions, [use C++ exceptions], yes, [USE_EXCEPTIONS])
AM_CONDITIONAL(USE_EXCEPTIONS, test "$enable_exceptions" = yes)
if test "$enable_exceptions" = no; then
	AC_ADD_CFLAG(CXXFLAGS, "-fno-exceptions")
fi
AH_TEMPLATE([USE_FIXEDPOINT], [use fixed point (instead of floating point) arithmetic])
AC_OPTION(fixedpoint, [use fixed point (instead of floating point) arithmetic], yes, [USE_FIXEDPOINT])
AH_TEMPLATE([SPLASH_CMYK], [include support for CMYK rasterization])
AC_OPTION(cmyk, [include support for CMYK rasterization], yes, [SPLASH_CMYK])
AH_TEMPLATE([NO_FONTCONFIG], [disable support for libfontconfig])
AC_OPTION(no-fontconfig, [disable support for libfontconfig], no, [NO_FONTCONFIG])
AH_TEMPLATE([SPLASH_DEVICEN], [Enable support for DeviceN output.])
AC_OPTION(devicen, [Enable support for DeviceN output.], yes, [SPLASH_DEVICEN])
AH_TEMPLATE([XPDFWIDGET_PRINTING], [include printing support in XpdfWidget])
AC_OPTION(xpdfwidget-printing, [include printing support in XpdfWidget], yes)
if test "$TARGET_OS" = "win32 -a ! -f "$srcdir/xpdf/WinPDFPrinter.cc -a "$enable_xpdfwidget_printing" = yes; then
	AC_MSG_WARN([printing support not available on this platform])
	enable_xpdfwidget_printing=no
fi
AM_CONDITIONAL(XPDFWIDGET_PRINTING, test "$enable_xpdfwidget_printing" = yes)


# HighlightFile.cc missing from source package?
AH_TEMPLATE([HIGHLIGHTED_REGIONS], [Enable support for highlighted regions.])
#AC_OPTION(highlighted-regions, [Enable support for highlighted regions.], yes)
AM_CONDITIONAL(HIGHLIGHTED_REGIONS, test "$enable_highlighted_regions" = yes)


AH_TEMPLATE([APPDEFDIR], [directory name for application defaults])
AC_ARG_WITH([appdef-dir],
  AS_HELP_STRING([--with-appdef-dir],
                 [set app-defaults directory]))
if test "$with_appdef_dir" != "no" -a "$with_appdef_dir" != "yes" -a "$with_appdef_dir" != ""; then
      AC_DEFINE_UNQUOTED(APPDEFDIR, "$with_appdef_dir")
fi

dnl ##### Path to xpdfrc.
dnl This ugly kludge to get the sysconfdir path is needed because
dnl autoconf does not actually set the prefix variable until later.
if test "$sysconfdir" = '${prefix}/etc'; then
  if test "x$prefix" = xNONE; then
    system_xpdfrc="$ac_default_prefix/etc/xpdfrc"
  else
    system_xpdfrc="$prefix/etc/xpdfrc"
  fi
else
  system_xpdfrc="$sysconfdir/xpdfrc"
fi
AC_DEFINE_UNQUOTED(SYSTEM_XPDFRC, "$system_xpdfrc", [directory for system wide xpdfrc])

AH_TEMPLATE([XPDFRC_DATADIR], [directory to use for the DATADIR xpdfrc variable])
AC_ARG_WITH([xpdfrc-datadir],
  AS_HELP_STRING([--with-xpdfrc-datadir],
                 [set directory to use for the DATADIR xpdfrc variable]))
if test "$with_xpdfrc_datadir" != "no" -a "$with_xpdfrc_datadir" != "yes" -a "$with_xpdfrc_datadir" != ""; then
      AC_DEFINE_UNQUOTED(XPDFRC_DATADIR, "$with_xpdfrc_datadir")
fi

AH_TEMPLATE([USE_FORMS], [Support Forms])
case $TARGET_OS in
  tos)
    ;;
  *) AC_DEFINE([USE_FORMS], 1)
    ;;
esac

###############################################################################
#
# check for libpng
#
###############################################################################

AH_TEMPLATE([HAVE_LIBPNG],
	    [Define this if the Portable Network Graphics library is installed.
	    It is basically required, but many things will more-or-less limp
	    along without it.])

PKG_CHECK_MODULES(PNG, libpng, have_libpng=yes, have_libpng=no)
if test "$have_libpng" = yes; then
  AC_DEFINE([HAVE_LIBPNG])
fi
AM_CONDITIONAL(HAVE_LIBPNG, test "$have_libpng" = yes)

###############################################################################
#
#       Check for freetype
#
###############################################################################

AH_TEMPLATE([HAVE_FREETYPE], [This is defined if using FreeType 2.])

PKG_CHECK_MODULES(FREETYPE2, freetype2, have_freetype=yes, have_freetype=no)
if test "$have_freetype" = yes; then
  AC_DEFINE([HAVE_FREETYPE])
  AC_DEFINE([HAVE_SPLASH], 1, [Defined if the Splash library is available.])
else
  AC_MSG_ERROR([freetype libraries not available])
fi
AM_CONDITIONAL(HAVE_FREETYPE, test "$have_freetype" = yes)
AM_CONDITIONAL(HAVE_SPLASH, test "$have_freetype" = yes)
dnl this can get wrong if PKG_CONFIG_PATH is not set
if test "$TARGET_OS" = tos; then
   FREETYPE2_LIBS="-lfreetype.slb -lpng.slb -lbz2.slb -lz.slb"
   FREETYPE2_CFLAGS="-DUSE_PRIVATE_FONTDIR -DFREETYPE_SLB -DPNGLIB_SLB -DZLIB_SLB -DBZIP2_SLB -I$(dirname $(dirname $($CC --print-file-name=libc.a)))/include/freetype2"
fi

###############################################################################
#
#       Check for lcms
#
###############################################################################

# ColorManager.cc missing from source package?
#PKG_CHECK_MODULES(LCMS, lcms2, have_lcms=yes, have_lcms=no)
have_lcms=no
AH_TEMPLATE([HAVE_LCMS], [Defined if using lcms2.])
AM_CONDITIONAL(HAVE_LCMS, test "$have_lcms" = yes)

###############################################################################
#
#       Check for qt5
#
###############################################################################

have_qt4=no
have_qt5=no
qt5_libs="Qt5Widgets Qt5Network Qt5Gui Qt5Core"
if test "$enable_xpdfwidget_printing" = yes; then
	qt5_libs="$qt5_libs Qt5PrintSupport"
fi
PKG_CHECK_MODULES(QT, [$qt5_libs], have_qt5=yes, have_qt5=no)
dnl this can get wrong if PKG_CONFIG_PATH is not set
if test "$TARGET_OS" = tos; then
  QT_LIBS=
  QT_CFLAGS=
  have_qt5=no
fi
# separate variable with defines and includes only,
# which is also passed to moc
QT_DEFS="$QT_CFLAGS"
case $TARGET_OS in
unix)
	if test "$have_qt5" = yes; then
		QT_CFLAGS="$QT_CFLAGS -fPIC -std=gnu++11"
		if test "$enable_xpdfwidget_printing" = yes; then
			QT_LIBS="$QT_LIBS -lcups"
		fi
	fi
	;;
mac)
	if test "$have_qt5" = yes; then
		if test "$enable_xpdfwidget_printing" = yes; then
			QT_LIBS="$QT_LIBS -framework ApplicationServices"
		fi
	fi
	;;
win32)
	;;
esac
if test "$have_qt5" = yes; then
	if test "$enable_xpdfwidget_printing" = yes; then
		QT_DEFS="$QT_DEFS -DXPDFWIDGET_PRINTING"
	fi
	MOC=moc-qt5
	RCC=rcc-qt5
fi
AC_SUBST(QT_CFLAGS)
AC_SUBST(QT_DEFS)
AC_SUBST(QT_LIBS)
AC_SUBST(MOC)
AC_SUBST(RCC)
AM_CONDITIONAL(HAVE_QT, test "$have_qt5" = yes -o "$have_qt4" = yes)

###############################################################################
#
#       look for libpaper
#
###############################################################################

AC_CHECK_LIB(paper, paperinit)
AC_CHECK_HEADERS([paper.h])

###############################################################################
#
#       look for fontconfig
#
###############################################################################
if test "$enable_no_fontconfig" != yes; then
  AC_CHECK_LIB(fontconfig, FcConfigCreate)
  AC_CHECK_HEADERS([fontconfig.h])
fi

###############################################################################
#
#       Check for std::sort.
#
###############################################################################

AH_TEMPLATE([HAVE_STD_SORT], [define if you have std::sort])
AC_CACHE_CHECK([for std::sort],
xpdf_cv_func_std_sort,
[AC_LANG_PUSH([C++])
AC_COMPILE_IFELSE(
  [AC_LANG_PROGRAM([[#include <algorithm>
struct functor {
  bool operator()(const int &i0, const int &i1) { return i0 < i1; }
};]],
                  [[int a[100];
std::sort(a, a+100, functor());]])],
xpdf_cv_func_std_sort=yes, xpdf_cv_func_std_sort=no)
AC_LANG_POP()])
if test "$xpdf_cv_func_std_sort" = yes; then
  AC_DEFINE(HAVE_STD_SORT)
fi

###############################################################################
#
###############################################################################

AH_TEMPLATE([EVAL_MODE], [Defined for evaluation mode.])
AH_TEMPLATE([BUILDING_XPDFREADER], [Defined when building the closed source XpdfReader binary.])

###############################################################################
#
#       Print some warnings at the end.
#
###############################################################################

warn_prefix_1="    Warning:"
warn_prefix_2="       Note:"
warn_prefix="$warn_prefix_1"

warning=no
warnsep='    #################################################################'

warnpre() {
  if test "$warning" = no ; then
    echo '' ; echo "$warnsep" ; echo ''
    warning=yes
  fi
}

warn() {
  warnpre
  if test "$warning" = long ; then echo '' ; fi
  warning=yes
  rest="$@"
  echo "$warn_prefix $rest"
}

warnL() {
  was=$warning
  warnpre
  warning=yes
  if test "$was" != no ; then echo '' ; fi
  rest="$@"
  echo "$warn_prefix $rest"
}

warn2() {
  rest="$@"
  echo "             $rest"
  warning=long
}

note() {
  warn_prefix="$warn_prefix_2"
  warn $@
  warn_prefix="$warn_prefix_1"
}

noteL() {
  warn_prefix="$warn_prefix_2"
  warnL $@
  warn_prefix="$warn_prefix_1"
}


if test "$have_png" = no ; then
  noteL 'The PNG library was not found.'
  warn2 "will not build pdftopng or pdftohtml."
fi

if test "$have_freetype" = no ; then
  noteL 'The freetype library was not found.'
  warn2 "will not build pdftoppm, pdftopng, pdftohtml, or xpdf."
fi

if test "$have_qt4" = no -a "$have_qt5" = no; then
  warnL 'Could not find Qt4 or Qt5.'
  warn2 'Will not build xpdf.'
fi

if test "$enable_multithreaded" = no; then
  warnL 'Multithreading is disabled.'
  warn2 'Will not build xpdf.'
fi

if test "$enable_exceptions" = no; then
  warnL 'C++ exceptions are disabled.'
  warn2 'Will not build xpdf.'
fi

if test "$warn_static_shared" != ""; then
  warnL "both --disable-static and --disable-shared given,"
  warn2 "using static libraries"
fi

if test "$warning" != no; then
  echo '' ; echo "$warnsep" ; echo ''
fi


###############################################################################
#
#       spit out Makefiles
#
###############################################################################

AC_CONFIG_COMMANDS_PRE([
if test "$GCC" = "yes" -a "$enable_fatal_warnings" = "yes"; then
	CFLAGS="$CFLAGS -Werror"
	CXXFLAGS="$CXXFLAGS -Werror"
	OBJCFLAGS="$OBJCFLAGS -Werror"
fi
])

AC_CONFIG_FILES([
	Makefile
	doc/Makefile
	fofi/Makefile
	goo/Makefile
	splash/Makefile
	xpdf/Makefile
	xpdf-qt/Makefile
])
AC_OUTPUT
