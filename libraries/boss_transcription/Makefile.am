lib_LTLIBRARIES = libbosstranscription.la libbosstranscriptionde.la libbosstranscriptionuk.la

libbosstranscription_la_SOURCES = boss_transcription.cpp 
libbosstranscription_la_LDFLAGS = -module -no-undefined -avoid-version
libbosstranscription_la_LIBADD = @LIB_XERCES_C@ @LIB_MYSQLCLIENT@ @LIB_PCRE@ ../boss_utility/libbossutility.la ../boss_g2p/libbossg2pde.la ../boss_mysql/libbossmysql.la ../tdsp/libtdsp.la ../boss_context/libbosscontext.la ../phimbl/libphimbl.la 


libbosstranscriptionde_la_SOURCES = boss_transcription_de.cpp boss_transcription_de.h boss_morphemes_de.cpp  boss_morphemes_de.h boss_morphtree.cpp boss_morphtree.h
libbosstranscriptionde_la_LDFLAGS = -module -no-undefined -avoid-version
libbosstranscriptionde_la_LIBADD = @LIB_XERCES_C@ @LIB_MYSQLCLIENT@ @LIB_PCRE@ ../boss_utility/libbossutility.la ../boss_g2p/libbossg2pde.la ../boss_mysql/libbossmysql.la ../tdsp/libtdsp.la ../boss_context/libbosscontext.la ../phimbl/libphimbl.la libbosstranscription.la

libbosstranscriptionuk_la_SOURCES = boss_transcription.cpp boss_transcription.h boss_transcription_uk.cpp boss_transcription_uk.h boss_morphemes_de.cpp  boss_morphemes_de.h boss_morphtree.cpp boss_morphtree.h
libbosstranscriptionuk_la_LDFLAGS = -module -no-undefined -avoid-version
libbosstranscriptionuk_la_LIBADD = @LIB_XERCES_C@ @LIB_MYSQLCLIENT@ @LIB_PCRE@ ../boss_utility/libbossutility.la ../boss_mysql/libbossmysql.la ../tdsp/libtdsp.la ../boss_context/libbosscontext.la ../phimbl/libphimbl.la

bin_PROGRAMS = boss_transcription_de
boss_transcription_de_SOURCES = main.cpp
#boss_transcription_de_LDADD = @LIB_XERCES_C@ @LIB_PTHREAD@ @LIB_GZ@ @LIB_MYSQLCLIENT@ libbosstranscriptionde.la ../boss_utility/libbossutility.la ../boss_g2p/libbossg2pde.la ../boss_mysql/libbossmysql.la ../tdsp/libtdsp.la ../boss_context/libbosscontext.la ../phimbl/libphimbl.la
boss_transcription_de_LDADD = @LIB_XERCES_C@ @LIB_MYSQLCLIENT@ @LIB_PCRE@ libbosstranscription.la libbosstranscriptionde.la ../boss_utility/libbossutility.la ../boss_g2p/libbossg2pde.la ../boss_mysql/libbossmysql.la ../tdsp/libtdsp.la ../boss_context/libbosscontext.la #../phimbl/libphimbl.la
AM_CPPFLAGS = -I..
 
bosstranscription_includedir=$(includedir)/boss_transcription
bosstranscription_include_HEADERS = boss_transcription.h

