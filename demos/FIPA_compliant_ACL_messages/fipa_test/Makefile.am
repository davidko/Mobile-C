AUTOMAKE_OPTIONS = gnu
noinst_PROGRAMS = server server2 client
INCLUDES = -I$(top_srcdir)/src/include -I$(top_srcdir)/src/mxml-2.2.2 -I$(top_srcdir)/src/mc_sync -I$(top_srcdir)/src/mc_list

server_SOURCES = server.c
server_LDADD = $(top_srcdir)/src/libmc.la 

server2_SOURCES = server2.c
server2_LDADD = $(top_srcdir)/src/libmc.la 

client_SOURCES = client.c
client_LDADD = $(top_srcdir)/src/libmc.la 
