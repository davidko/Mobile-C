AUTOMAKE_OPTS = gnu
noinst_PROGRAMS = server client
INCLUDES = -I$(top_srcdir)/src/include -I$(top_srcdir)/src/mxml-2.2.2 -I$(top_srcdir)/src/mc_sync -I$(top_srcdir)/src/mc_list

server_SOURCES = server.c
server_LDADD = $(top_srcdir)/src/libmc.la -lm  
server_LIBS = -lpthread -lmc -ldl

client_SOURCES = client.c
client_LDADD = $(top_srcdir)/src/libmc.la -lm  
client_LIBS = -lpthread -lmc -ldl
