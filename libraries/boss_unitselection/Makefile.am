lib_LTLIBRARIES = libbossunitselection.la libunitselectionfactoryde.la
libbossunitselection_la_SOURCES = factory.cpp unit_level.cpp boss_node.cpp boss_unitselection.cpp
libbossunitselection_la_LDFLAGS = -module -no-undefined -avoid-version
libbossunitselection_la_LIBADD = @LIB_LTDL@ @LIB_XERCES_C@ @LIB_MYSQLCLIENT@ ../boss_mysql/libbossmysql.la ../boss_utility/libbossutility.la ../tdsp/libtdsp.la

libunitselectionfactoryde_la_SOURCES = factory_de.cpp cost_de.cpp boss_node.cpp boss_node_de.cpp 
libunitselectionfactoryde_la_LIBADD = @LIB_XERCES_C@ @LIB_MYSQLCLIENT@ ../boss_mysql/libbossmysql.la ../boss_utility/libbossutility.la ../tdsp/libtdsp.la
#libunitselectionfactoryde_la_LDFLAGS = -module -no-undefined -avoid-version

AM_CPPFLAGS = -I..

bossunitselection_includedir=$(includedir)/boss_unitselection
bossunitselection_include_HEADERS = boss_node.h boss_unitselection.h cost.h factory.h unit_level.h 
