AUTOMAKE_OPTIONS = gnu
lib_LTLIBRARIES = libxyssl.la
libxyssl_la_SOURCES = aes.c \
									 arc4.c \
									 base64.c \
									 bignum.c \
									 certs.c \
									 debug.c \
									 des.c \
									 dhm.c \
									 havege.c \
									 md2.c \
									 md4.c \
									 md5.c \
									 net.c \
									 padlock.c \
									 rsa.c \
									 sha1.c \
									 sha2.c \
									 sha4.c \
									 ssl_cli.c \
									 ssl_srv.c \
									 ssl_tls.c \
									 timing.c \
									 x509parse.c

libxyssl_la_LDFLAGS = -rdynamic
INCLUDES = -I../include

libxyssl_la_LDFLAGS = -version-info 0:0:0
libxyssl_la_CFLAGS = -DSELF_TEST -DHAVE_RDTSC
