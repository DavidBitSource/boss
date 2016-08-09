#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(boss, 3.00, http://www.ikp.uni-bonn.de/boss)
AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([.])
AC_CONFIG_HEADER(config.h)
AC_LIBTOOL_DLOPEN

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AM_PROG_LEX
AC_PROG_YACC
AC_PROG_INSTALL
AC_PROG_MAKE_SET
#AC_PROG_RANLIB
AC_PROG_LIBTOOL

# Checks for libraries.
#
# Check for Bison. 
#
# This macro verifies that Bison is installed.  If successful, then
# 1) YACC is set to bison -y (to emulate YACC calls)
# 2) BISON is set to bison
#
AC_DEFUN([AC_PROG_BISON],
[AC_CACHE_CHECK([],[llvm_cv_has_bison],[AC_PROG_YACC()])
if test "$YACC" != "bison -y"; then
  AC_MSG_ERROR([bison not found but required])
else
  AC_SUBST(BISON,[bison],[location of bison])
fi])

AC_CHECK_LIB([c], [accept], [echo "socket functions found in libc."] , AC_CHECK_LIB([socket], [accept], [AC_SUBST(LIB_SOCKET, -lsocket)], [AC_MSG_ERROR([socket functions not found in either libc or libsocket. Aborting...])]))
#AC_SUBST(LIB_GZ, /usr/lib/libgz.so.1)
AC_CHECK_LIB([gz], [deflate], [AC_SUBST(LIB_GZ, -lgz)], [AC_CHECK_LIB([z],[deflate],[AC_SUBST(LIB_GZ, -lz)],[AC_MSG_ERROR([Sorry, libgz (gzip library) is required])])])
AC_CHECK_LIB([m], [round], [AC_SUBST(LIB_M, -lm)], [AC_MSG_ERROR([Sorry, libm (math library) is required])])
AC_CHECK_LIB([mysqlclient], [mysql_fetch_row], [AC_SUBST(LIB_MYSQLCLIENT, -lmysqlclient)], [AC_MSG_ERROR([Sorry, libmysqlclient (sql library) is required])])
#AC_CHECK_LIB([nsl], [gethostbyname], [AC_SUBST(LIB_NSL, -lnsl)], [AC_MSG_ERROR([Sorry, libnsl (network library) is required])])
AC_CHECK_LIB([nsl], [gethostbyname], [AC_SUBST(LIB_NSL, -lnsl)], AC_CHECK_LIB([cygwin], [gethostbyname], [AC_SUBST(LIB_NSL, -lcygwin)]))
AC_CHECK_LIB([pthread], [pthread_create], [AC_SUBST(LIB_PTHREAD, -lpthread)], [AC_MSG_ERROR([Sorry, libpthread (thread library) is required])])
AC_CHECK_LIB([xerces-c], [main], [AC_SUBST(LIB_XERCES_C, -lxerces-c)], [AC_MSG_ERROR([Sorry, libxerces-c (xml library) is required])])
AC_CHECK_LIB([pcre], [pcre_dfa_exec], [AC_SUBST(LIB_PCRE, -lpcre)], [AC_MSG_ERROR([Sorry, pcre is required])])

#AC_CHECK_LIB(fl, yywrap,[AC_SUBST(LIB_LEX, -lfl)], [AC_MSG_ERROR([Sorry, libfl (flex library) is required])])
# fftw functions should be replaced with tdsp functions
#AC_CHECK_LIB([dfftw], [fftw], [AC_SUBST(LIB_DFFTW, -ldfftw)], [AC_MSG_ERROR([Sorry, libfftw (fft library) is required])])
#AC_CHECK_LIB([drfftw], [rfftw_create_plan], [AC_SUBST(LIB_DRFFTW, -ldrfftw)], [AC_MSG_ERROR([Sorry, librfftw (fft library) is required])])
AC_CHECK_LIB([ltdl], [lt_dlopen], [AC_SUBST(LIB_LTDL, -lltdl)], [AC_MSG_ERROR([Sorry, libltdl (libtool dynamic loading library) is required])])


# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h locale.h netdb.h netinet/in.h stdlib.h sys/socket.h unistd.h])
AC_SUBST(CPPFLAGS, "-I /usr/include/mysql")


# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_TYPE_SIGNAL
AC_FUNC_STRFTIME
AC_CHECK_FUNCS([floor gethostbyname gethostname inet_ntoa memset pow regcomp rint setlocale socket sqrt])

AC_CONFIG_FILES([Makefile
		libraries/Makefile
		libraries/tdsp/Makefile
		libraries/boss_utility/Makefile
		libraries/boss_mysql/Makefile
		libraries/boss_cscomm/Makefile
		libraries/boss_numbers/Makefile
		libraries/boss_context/Makefile
		libraries/phimbl/Makefile
    		libraries/boss_g2p/Makefile
		libraries/boss_transcription/Makefile
                libraries/boss_cartreader/Makefile
		libraries/boss_duration/Makefile
		libraries/boss_intonation/Makefile
		libraries/boss_unitselection/Makefile
		libraries/boss_conman/Makefile
		libraries/preproc/Makefile
		tools/Makefile
		tools/traing2p/Makefile
		tools/blf2xml/Makefile
		tools/optbounds/Makefile
		tools/addcontext/Makefile
		tools/addphrases/Makefile
		tools/addhp/Makefile
		tools/alignflag/Makefile
		tools/melbounds/Makefile
		tools/addf0/Makefile
		tools/blfxml2db/Makefile
		tools/blf2db/Makefile
		synthesis/Makefile
		synthesis/boss_server/Makefile
		synthesis/boss_client/Makefile
])
#synthesis/boss_client/Makefile
AC_OUTPUT()
