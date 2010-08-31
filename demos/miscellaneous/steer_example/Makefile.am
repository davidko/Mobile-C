AUTOMAKE_OPTS = gnu
noinst_PROGRAMS = steer_command server 
INCLUDES = -I$(top_srcdir)/src/include -I$(top_srcdir)/src/mxml-2.2.2 -I$(top_srcdir)/src/mc_sync -I$(top_srcdir)/src/mc_list

steer_command_SOURCES = steer_command.c
steer_command_LDADD = $(top_srcdir)/src/libmc.la -lm  
steer_command_LDFLAGS = @LDFLAGS@ -rdynamic
steer_command_LIBS = -lpthread -lchsdk -lmc -lembedch -ldl
steer_command_CFLAGS=-I../../source/include

server_SOURCES = server.c
server_LDADD = $(top_srcdir)/src/libmc.la -lm  
server_LDFLAGS = @LDFLAGS@ -rdynamic
server_LIBS = -lpthread -lchsdk -lmc -lembedch -ldl
server_CFLAGS=-I../../source/include

