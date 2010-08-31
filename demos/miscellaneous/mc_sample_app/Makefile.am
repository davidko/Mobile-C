AUTOMAKE_OPTS = gnu
noinst_PROGRAMS = server client
bin_PROGRAMS = mc_sample_app
INCLUDES = -I$(top_srcdir)/src/include -I$(top_srcdir)/src/mxml-2.2.2 -I$(top_srcdir)/src/mc_sync -I$(top_srcdir)/src/mc_list
mc_sample_app_SOURCES = mc_sample_app.c
mc_sample_app_LDADD = $(top_srcdir)/src/libmc.la -lm  
mc_sample_app_LDFLAGS = @LDFLAGS@ -rdynamic
mc_sample_app_LIBS = -lpthread -lchsdk -lmc -lembedch -ldl
mc_sample_app_CFLAGS=-I../../source/include

server_SOURCES = server.c
server_LDADD = $(top_srcdir)/src/libmc.la -lm  
server_LDFLAGS = @LDFLAGS@ -rdynamic
server_LIBS = -lpthread -lchsdk -lmc -lembedch -ldl
server_CFLAGS=-I../../source/include

client_SOURCES = client.c
client_LDADD = $(top_srcdir)/src/libmc.la -lm  
client_LDFLAGS = @LDFLAGS@ -rdynamic
client_LIBS = -lpthread -lchsdk -lmc -lembedch -ldl
client_CFLAGS=-I../../source/include
