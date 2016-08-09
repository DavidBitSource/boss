lib_LTLIBRARIES = libbossutility.la
libbossutility_la_SOURCES = boss_types.cpp boss_types.h boss_utility.cpp boss_utility.h commandline.cpp commandline.h boss_config.cpp boss_config.h dom_tools.cpp dom_tools.h dom_userdata.cpp dom_userdata.h boss_fsa.cpp boss_fsa.h
libbossutility_la_LIBADD = @LIB_XERCES_C@ @LIB_MYSQLCLIENT@ @LIB_PCRE@ ../../libraries/boss_mysql/libbossmysql.la 
libbossutility_la_LDFLAGS = -no-undefined
AM_CPPFLAGS = -I..


bossutility_includedir=$(includedir)/boss_utility
bossutility_include_HEADERS = boss_config.h boss_exceptions.h boss_fsa.h boss_module.h boss_optitem.h boss_types.h boss_utility.h commandline.h dom_tools.h dom_userdata.h
