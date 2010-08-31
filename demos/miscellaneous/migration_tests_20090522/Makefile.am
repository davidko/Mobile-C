AUTOMAKE_OPTIONS = gnu
noinst_PROGRAMS = client server1 server2 server3 migration_test_client migration_test_server
INCLUDES = -I$(top_srcdir)/src/include -I$(top_srcdir)/src/mxml-2.2.2 -I$(top_srcdir)/src/mc_sync -I$(top_srcdir)/src/mc_list

client_SOURCES = client.c
client_LDADD = $(top_srcdir)/src/libmc.la 

server1_SOURCES = server1.c
server1_LDFLAGS=@LDFLAGS@ -rdynamic 
server1_LDADD = $(top_srcdir)/src/libmc.la 

server2_SOURCES = server2.c
server2_LDFLAGS=@LDFLAGS@ -rdynamic 
server2_LDADD = $(top_srcdir)/src/libmc.la 

server3_SOURCES = server3.c
server3_LDFLAGS=@LDFLAGS@ -rdynamic 
server3_LDADD = $(top_srcdir)/src/libmc.la 

migration_test_client_SOURCES = migration_test_client.c
migration_test_client_LDFLAGS=@LDFLAGS@ -rdynamic 
migration_test_client_LDADD = $(top_srcdir)/src/libmc.la 

migration_test_server_SOURCES = migration_test_server.c
migration_test_server_LDFLAGS=@LDFLAGS@ -rdynamic 
migration_test_server_LDADD = $(top_srcdir)/src/libmc.la 
