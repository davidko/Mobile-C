AUTOMAKE_OPTS = gnu
noinst_PROGRAMS = server1 server2 server3 client
INCLUDES = -I$(top_srcdir)/src/include -I$(top_srcdir)/src/mxml-2.2.2 -I$(top_srcdir)/src/mc_sync -I$(top_srcdir)/src/mc_list

server1_SOURCES = server1.c
server1_LDADD = $(top_srcdir)/src/libmc.la -lm  
server1_LDFLAGS = @LDFLAGS@ -rdynamic
server1_LIBS = -lpthread -lchsdk -lmc -lembedch -ldl
server1_CFLAGS=-I../../src/include

server2_SOURCES = server2.c
server2_LDADD = $(top_srcdir)/src/libmc.la -lm  
server2_LDFLAGS = @LDFLAGS@ -rdynamic
server2_LIBS = -lpthread -lchsdk -lmc -lembedch -ldl
server2_CFLAGS=-I../../src/include

server3_SOURCES = server3.c
server3_LDADD = $(top_srcdir)/src/libmc.la -lm  
server3_LDFLAGS = @LDFLAGS@ -rdynamic
server3_LIBS = -lpthread -lchsdk -lmc -lembedch -ldl
server3_CFLAGS=-I../../src/include

client_SOURCES = client.c
client_LDADD = $(top_srcdir)/src/libmc.la -lm  
client_LDFLAGS = @LDFLAGS@ -rdynamic
client_LIBS = -lpthread -lchsdk -lmc -lembedch -ldl
client_CFLAGS=-I../../src/include
