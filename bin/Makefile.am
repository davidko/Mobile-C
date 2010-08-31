AUTOMAKE_OPTIONS = gnu
noinst_PROGRAMS = mc_genkey

mc_genkey_SOURCES = ../src/util/mc_genkey.c \
                    ../src/security/interface.c

mc_genkey_LDADD = $(top_srcdir)/src/security/xyssl-0.9/library/libxyssl.la
