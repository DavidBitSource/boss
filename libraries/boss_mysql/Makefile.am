lib_LTLIBRARIES = libbossmysql.la
libbossmysql_la_SOURCES = boss_mysql_exceptions.cpp boss_mysql_exceptions.h boss_mysql_result.cpp boss_mysql_result.h boss_mysql_handler.cpp boss_mysql_handler.h
libbossmysql_la_LDFLAGS = -no-undefined
libbossmysql_la_LIBADD = @LIB_XERCES_C@ @LIB_MYSQLCLIENT@
AM_CPPFLAGS = -I..

bossmysql_includedir=$(includedir)/boss_mysql
bossmysql_include_HEADERS = boss_mysql_handler.h boss_mysql_exceptions.h \
			    boss_mysql_result.h


