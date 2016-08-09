lib_LTLIBRARIES = libbossnumbers.la
libbossnumbers_la_SOURCES = boss_numbers.cpp boss_numbers.h nominal.cpp nominal.h ordinal.cpp ordinal.h
bin_PROGRAMS = boss_numbers
boss_numbers_SOURCES = main.cpp
LDADD = libbossnumbers.la ../../libraries/boss_utility/libbossutility.la ../../libraries/boss_mysql/libbossmysql.la @LIB_XERCES_C@ @LIB_MYSQLCLIENT@ @LIB_PCRE@
AM_CPPFLAGS = -I..
