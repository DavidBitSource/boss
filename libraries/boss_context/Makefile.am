lib_LTLIBRARIES = libbosscontext.la
libbosscontext_la_SOURCES = boss_context.cpp boss_context.h
libbosscontext_la_LDFLAGS = -no-undefined

bin_PROGRAMS = boss_context
boss_context_SOURCES = main.cpp
LDADD = libbosscontext.la ../../libraries/boss_utility/libbossutility.la ../../libraries/boss_mysql/libbossmysql.la @LIB_XERCES_C@ @LIB_MYSQLCLIENT@ @LIB_PCRE@
AM_CPPFLAGS = -I..

bosscontext_includedir=$(includedir)/boss_context
bosscontext_include_HEADERS = boss_context.h
