AC_PREREQ(2.61)
AC_INIT
AC_CONFIG_AUX_DIR([.])
AM_INIT_AUTOMAKE(c_client, 4.2.0)

CF_ORG="$CFLAGS"
CX_ORG="$CXXFLAGS"

AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CXX
AM_PROG_INSTALL_STRIP

LT_INIT
AC_ENABLE_SHARED
AC_DISABLE_STATIC
AC_CONFIG_MACRO_DIR([m4])

AC_DEFINE(GS_DLL_INSIDE,)
AC_DEFINE(GS_EXPERIMENTAL_TOOL_ENABLED,1)
AC_DEFINE(GS_ERROR_COMMON_ONLY,1)

CF_COMMON="-Wall -finput-charset=UTF-8"
CX_COMMON="$CF_COMMON"

CF_OPTIMIZE=" -O3 -fno-tree-vectorize -DNDEBUG"
AC_ARG_ENABLE(debug_plain, [  --enable-debug-plain    turn on debugging],
[ test "${enableval}" = "yes" && CF_OPTIMIZE=" -g -O0" ])
AC_ARG_ENABLE(debug, [  --enable-debug    turn on debugging],
[ test "${enableval}" = "yes" && CF_OPTIMIZE=" -gstabs+ -O0" ])
CX_OPTIMIZE="$CF_OPTIMIZE"

CF_STRICT=""
CX_STRICT=""
AC_ARG_ENABLE(strict, [  --enable-strict    turn on strict compiling],
[ test "${enableval}" = "yes" &&
CF_STRICT=" \
 -Wextra -Wformat=2 -Wstrict-aliasing=2 -Wcast-qual -Wcast-align\
 -Wwrite-strings -Wswitch-enum -Wconversion -Wfloat-equal -Wpointer-arith\
 -Wshadow -Winline" &&
CX_STRICT="$CF_STRICT -Woverloaded-virtual -Weffc++\
 -Wsign-promo -Wsynth -Wold-style-cast" ])

CFLAGS="$CF_ORG $CF_COMMON$CF_OPTIMIZE$CF_STRICT"
CXXFLAGS="$CX_ORG $CX_COMMON$CX_OPTIMIZE$CX_STRICT"

AC_CONFIG_FILES([
 Makefile
 main/Makefile])
AC_OUTPUT
