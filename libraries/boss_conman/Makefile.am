lib_LTLIBRARIES = libbossconman.la libbossconcat.la 

libbossconman_la_SOURCES = conman_unit.cpp
libbossconman_la_LIBADD = @LIB_XERCES_C@ @LIB_MYSQLCLIENT@ ../boss_mysql/libbossmysql.la ../boss_utility/libbossutility.la ../tdsp/libtdsp.la
libbossconman_la_LDFLAGS = -no-undefined -avoid-version

libbossconcat_la_SOURCES = boss_concat.cpp concat_unit.cpp
libbossconcat_la_LIBADD = @LIB_XERCES_C@ @LIB_MYSQLCLIENT@ ../boss_mysql/libbossmysql.la ../boss_utility/libbossutility.la ../boss_conman/libbossconman.la ../tdsp/libtdsp.la
libbossconcat_la_LDFLAGS = -no-undefined -avoid-version

AM_CPPFLAGS = -I..

bossconman_includedir=$(includedir)/boss_conman
bossconman_include_HEADERS = boss_conman.h conman_unit.h
