AUTOMAKE_OPTIONS = gnu
noinst_PROGRAMS = client \
			   server
INCLUDES = -I$(top_srcdir)/src/include -I$(top_srcdir)/src/mxml-2.2.2 -I$(top_srcdir)/src/mc_sync -I$(top_srcdir)/src/mc_list

client_SOURCES = client.c
client_LDADD = $(top_srcdir)/src/libmc.la 

server_SOURCES = server.c
server_LDFLAGS=@LDFLAGS@ -rdynamic 
server_LDADD = $(top_srcdir)/src/libmc.la 
AUTOMAKE_OPTIONS = gnu
noinst_PROGRAMS = client \
			   server
INCLUDES = -I$(top_srcdir)/src/include -I$(top_srcdir)/src/mxml-2.2.2 -I$(top_srcdir)/src/mc_sync -I$(top_srcdir)/src/mc_list

client_SOURCES = client.c
client_LDADD = $(top_srcdir)/src/libmc.la 

server_SOURCES = server.c
server_LDFLAGS=@LDFLAGS@ -rdynamic 
server_LDADD = $(top_srcdir)/src/libmc.la 
