bin_PROGRAMS = boss_server
boss_server_SOURCES = main.cpp boss_synthesis.cpp
## TODO: Reimplement using tdsp instead of fftw library:
## boss_pdspectrum.cpp
boss_server_LDADD = ../../libraries/boss_cscomm/libbosscscomm.la ../../libraries/boss_utility/libbossutility.la ../../libraries/boss_mysql/libbossmysql.la ../../libraries/tdsp/libtdsp.la @LIB_XERCES_C@ @LIB_PTHREAD@ @LIB_GZ@ @LIB_MYSQLCLIENT@ @LIB_NSL@ @LIB_LTDL@ @LIB_PCRE@
## @LIB_RFFTW@ @LIB_FFTW@
AM_CPPFLAGS = -I../../libraries
