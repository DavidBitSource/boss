lib_LTLIBRARIES = libbossduration.la libbossdurationde.la 
libbossduration_la_SOURCES =  boss_duration.cpp
libbossduration_la_LDFLAGS = -module -no-undefined -avoid-version
libbossduration_la_LIBADD = @LIB_XERCES_C@ @LIB_MYSQLCLIENT@ ../boss_utility/libbossutility.la ../boss_cartreader/libbosscartreader.la
libbossdurationde_la_SOURCES = boss_duration_de.cpp
libbossdurationde_la_LDFLAGS = -module -no-undefined -avoid-version
libbossdurationde_la_LIBADD = @LIB_XERCES_C@ @LIB_MYSQLCLIENT@ ../boss_utility/libbossutility.la ../boss_cartreader/libbosscartreader.la libbossduration.la
AM_CPPFLAGS = -I.. 

bossduration_includedir=$(includedir)/boss_duration
bossduration_include_HEADERS = boss_duration.h