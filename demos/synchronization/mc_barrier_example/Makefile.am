AUTOMAKE_OPTS = gnu
noinst_PROGRAMS = client server
INCLUDES = -I$(top_srcdir)/src/include -I$(top_srcdir)/src/mxml-2.2.2 -I$(top_srcdir)/src/mc_sync -I$(top_srcdir)/src/mc_list


client_SOURCES = client.c
client_LDADD = $(top_srcdir)/src/libmc.la -lm  
client_LDFLAGS = @LDFLAGS@ -rdynamic
server_SOURCES = server.c
server_LDADD = $(top_srcdir)/src/libmc.la -lm  
server_LDFLAGS = @LDFLAGS@ -rdynamic
client_LIBS = -lpthread -lmc -lembedch -lchsdk -ldl
server_LIBS = -lpthread -lmc -lembedch -lchsdk -ldl
client_CFLAGS=-I../../source/include
server_CFLAGS=-I../../source/include
