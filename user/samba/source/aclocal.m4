dnl AC_VALIDATE_CACHE_SYSTEM_TYPE[(cmd)]
dnl if the cache file is inconsistent with the current host,
dnl target and build system types, execute CMD or print a default
dnl error message.
AC_DEFUN(AC_VALIDATE_CACHE_SYSTEM_TYPE, [
    AC_REQUIRE([AC_CANONICAL_SYSTEM])
    AC_MSG_CHECKING([config.cache system type])
    if { test x"${ac_cv_host_system_type+set}" = x"set" &&
         test x"$ac_cv_host_system_type" != x"$host"; } ||
       { test x"${ac_cv_build_system_type+set}" = x"set" &&
         test x"$ac_cv_build_system_type" != x"$build"; } ||
       { test x"${ac_cv_target_system_type+set}" = x"set" &&
         test x"$ac_cv_target_system_type" != x"$target"; }; then
	AC_MSG_RESULT([different])
	ifelse($#, 1, [$1],
		[AC_MSG_ERROR(["you must remove config.cache and restart configure"])])
    else
	AC_MSG_RESULT([same])
    fi
    ac_cv_host_system_type="$host"
    ac_cv_build_system_type="$build"
    ac_cv_target_system_type="$target"
])

dnl based on Automake's maintainer mode
AC_DEFUN(SAMBA_MAINTAINER_MODE,[
  AC_ARG_ENABLE(maintainer-mode,
	[  --enable-maintainer-mode enable some make rules for maintainers],
      maint_mode=$enableval, maint_mode=no)
  if test x"$maint_mode" = x"yes"; then MAINT=; else MAINT='#'; fi
  AC_SUBST(MAINT)
  AC_PATH_PROG(AUTOCONF, autoconf, autoconf)
  AC_SUBST(AUTOCONF)
  AC_PATH_PROG(AUTOHEADER, autoheader, autoheader)
  AC_SUBST(AUTOHEADER)
])


dnl test whether dirent has a d_off member
AC_DEFUN(AC_DIRENT_D_OFF,
[AC_CACHE_CHECK([for d_off in dirent], ac_cv_dirent_d_off,
[AC_TRY_COMPILE([
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>], [struct dirent d; d.d_off;],
ac_cv_dirent_d_off=yes, ac_cv_dirent_d_off=no)])
if test $ac_cv_dirent_d_off = yes; then
  AC_DEFINE(HAVE_DIRENT_D_OFF)
fi
])


dnl AC_PROG_CC_FLAG(flag)
AC_DEFUN(AC_PROG_CC_FLAG,
[AC_CACHE_CHECK(whether ${CC-cc} accepts -$1, ac_cv_prog_cc_$1,
[echo 'void f(){}' > conftest.c
if test -z "`${CC-cc} -$1 -c conftest.c 2>&1`"; then
  ac_cv_prog_cc_$1=yes
else
  ac_cv_prog_cc_$1=no
fi
rm -f conftest*
])])

dnl check for a function in a library, but don't
dnl keep adding the same library to the LIBS variable.
dnl AC_LIBTESTFUNC(lib,func)
AC_DEFUN(AC_LIBTESTFUNC,
[case "$LIBS" in
  *-l$1*) AC_CHECK_FUNCS($2) ;;
  *) AC_CHECK_LIB($1, $2) 
     AC_CHECK_FUNCS($2)
  ;;
  esac
])

# AC_CHECK_LIB_EXT(LIBRARY, [EXT_LIBS], [FUNCTION],
#              [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND],
#              [ADD-ACTION-IF-FOUND],[OTHER-LIBRARIES])
# ------------------------------------------------------
#
# Use a cache variable name containing both the library and function name,
# because the test really is for library $1 defining function $3, not
# just for library $1.  Separate tests with the same $1 and different $3s
# may have different results.
#
# Note that using directly AS_VAR_PUSHDEF([ac_Lib], [ac_cv_lib_$1_$3])
# is asking for troubles, since AC_CHECK_LIB($lib, fun) would give
# ac_cv_lib_$lib_fun, which is definitely not what was meant.  Hence
# the AS_LITERAL_IF indirection.
#
# FIXME: This macro is extremely suspicious.  It DEFINEs unconditionnally,
# whatever the FUNCTION, in addition to not being a *S macro.  Note
# that the cache does depend upon the function we are looking for.
#
# It is on purpose we used `ac_check_lib_ext_save_LIBS' and not just
# `ac_save_LIBS': there are many macros which don't want to see `LIBS'
# changed but still want to use AC_CHECK_LIB_EXT, so they save `LIBS'.
# And ``ac_save_LIBS' is too tempting a name, so let's leave them some
# freedom.
AC_DEFUN([AC_CHECK_LIB_EXT],
[
AH_CHECK_LIB_EXT([$1])
ac_check_lib_ext_save_LIBS=$LIBS
LIBS="-l$1 $$2 $7 $LIBS"
AS_LITERAL_IF([$1],
      [AS_VAR_PUSHDEF([ac_Lib_ext], [ac_cv_lib_ext_$1])],
      [AS_VAR_PUSHDEF([ac_Lib_ext], [ac_cv_lib_ext_$1''])])dnl

m4_ifval([$3],
 [
    AH_CHECK_FUNC_EXT([$3])
    AS_LITERAL_IF([$1],
              [AS_VAR_PUSHDEF([ac_Lib_func], [ac_cv_lib_ext_$1_$3])],
              [AS_VAR_PUSHDEF([ac_Lib_func], [ac_cv_lib_ext_$1''_$3])])dnl
    AC_CACHE_CHECK([for $3 in -l$1], ac_Lib_func,
	[AC_TRY_LINK_FUNC($3,
                 [AS_VAR_SET(ac_Lib_func, yes);
		  AS_VAR_SET(ac_Lib_ext, yes)],
                 [AS_VAR_SET(ac_Lib_func, no);
		  AS_VAR_SET(ac_Lib_ext, no)])
	])
    AS_IF([test AS_VAR_GET(ac_Lib_func) = yes],
        [AC_DEFINE_UNQUOTED(AS_TR_CPP(HAVE_$3))])dnl
    AS_VAR_POPDEF([ac_Lib_func])dnl
 ],[
    AC_CACHE_CHECK([for -l$1], ac_Lib_ext,
	[AC_TRY_LINK_FUNC([main],
                 [AS_VAR_SET(ac_Lib_ext, yes)],
                 [AS_VAR_SET(ac_Lib_ext, no)])
	])
 ])
LIBS=$ac_check_lib_ext_save_LIBS

AS_IF([test AS_VAR_GET(ac_Lib_ext) = yes],
    [m4_default([$4], 
        [AC_DEFINE_UNQUOTED(AS_TR_CPP(HAVE_LIB$1))
		case "$$2" in
		    *-l$1*)
			;;
		    *)
			$2="$$2 -l$1"
			;;
		esac])
		[$6]
	    ],
	    [$5])dnl
AS_VAR_POPDEF([ac_Lib_ext])dnl
])# AC_CHECK_LIB_EXT

# AH_CHECK_LIB_EXT(LIBNAME)
# ---------------------
m4_define([AH_CHECK_LIB_EXT],
[AH_TEMPLATE(AS_TR_CPP(HAVE_LIB$1),
             [Define to 1 if you have the `]$1[' library (-l]$1[).])])

# AC_CHECK_FUNCS_EXT(FUNCTION, [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# -----------------------------------------------------------------
dnl check for a function in a $LIBS and $OTHER_LIBS libraries variable.
dnl AC_CHECK_FUNC_EXT(func,OTHER_LIBS,IF-TRUE,IF-FALSE)
AC_DEFUN([AC_CHECK_FUNC_EXT],
[
    AH_CHECK_FUNC_EXT($1)	
    ac_check_func_ext_save_LIBS=$LIBS
    LIBS="$2 $LIBS"
    AS_VAR_PUSHDEF([ac_var], [ac_cv_func_ext_$1])dnl
    AC_CACHE_CHECK([for $1], ac_var,
	[AC_LINK_IFELSE([AC_LANG_FUNC_LINK_TRY([$1])],
                [AS_VAR_SET(ac_var, yes)],
                [AS_VAR_SET(ac_var, no)])])
    LIBS=$ac_check_func_ext_save_LIBS
    AS_IF([test AS_VAR_GET(ac_var) = yes], 
	    [AC_DEFINE_UNQUOTED(AS_TR_CPP([HAVE_$1])) $3], 
	    [$4])dnl
AS_VAR_POPDEF([ac_var])dnl
])# AC_CHECK_FUNC

# AH_CHECK_FUNC_EXT(FUNCNAME)
# ---------------------
m4_define([AH_CHECK_FUNC_EXT],
[AH_TEMPLATE(AS_TR_CPP(HAVE_$1),
             [Define to 1 if you have the `]$1[' function.])])

dnl Define an AC_DEFINE with ifndef guard.
dnl AC_N_DEFINE(VARIABLE [, VALUE])
define(AC_N_DEFINE,
[cat >> confdefs.h <<\EOF
[#ifndef] $1
[#define] $1 ifelse($#, 2, [$2], $#, 3, [$2], 1)
[#endif]
EOF
])

dnl Add an #include
dnl AC_ADD_INCLUDE(VARIABLE)
define(AC_ADD_INCLUDE,
[cat >> confdefs.h <<\EOF
[#include] $1
EOF
])
