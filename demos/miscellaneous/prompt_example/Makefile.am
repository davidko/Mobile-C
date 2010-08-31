AUTOMAKE_OPTIONS = gnu
noinst_PROGRAMS = server1 server2
INCLUDES = -I$(top_srcdir)/src/include -I$(top_srcdir)/src/mxml-2.2.2 -I$(top_srcdir)/src/mc_sync -I$(top_srcdir)/src/mc_list

server1_SOURCES = server1.c
server1_LDADD = $(top_srcdir)/src/libmc.la 

server2_SOURCES = server2.c
server2_LDFLAGS=@LDFLAGS@ -rdynamic 
server2_LDADD = $(top_srcdir)/src/libmc.la 
