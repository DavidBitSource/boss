lib_LTLIBRARIES = libbossg2pde.la
libbossg2pde_la_SOURCES = boss_g2p.cpp boss_g2p.h boss_g2p_de.cpp boss_g2p_de.h
libbossg2pde_la_LDFLAGS = -no-undefined
libbossg2pde_la_LIBADD = @LIB_XERCES_C@ ../boss_utility/libbossutility.la ../phimbl/libphimbl.la ../boss_mysql/libbossmysql.la
bin_PROGRAMS = boss_g2p_de
boss_g2p_de_SOURCES = main.cpp
# LDADD = libbossg2pde.la ../boss_utility/libbossutility.la ../phimbl/libphimbl.la @LIB_XERCES_C@ @LIB_MYSQLCLIENT@
LDADD = libbossg2pde.la ../boss_utility/libbossutility.la ../phimbl/libphimbl.la ../boss_mysql/libbossmysql.la @LIB_XERCES_C@ @LIB_PCRE@
AM_CPPFLAGS = -I..

bossg2p_includedir=$(includedir)/boss_g2p
bossg2p_include_HEADERS = boss_g2p.h
