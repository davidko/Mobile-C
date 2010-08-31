!include "../../../windows.mak"

CFLAGS=$(CFLAGSDEBUG) /I..\include
#CFLAGS=$(CFLAGSRELEASE)

LFLAGS=$(LFLAGSDEBUG)
#LFLAGS=$(LFLAGSRELEASE)	

OBJS=aes.obj arc4.obj base64.obj bignum.obj certs.obj debug.obj des.obj dhm.obj havege.obj md2.obj \
md4.obj md5.obj net.obj padlock.obj rsa.obj sha1.obj sha2.obj sha4.obj ssl_cli.obj ssl_srv.obj \
ssl_tls.obj timing.obj x509parse.obj 

all:xyssl.lib

aes.obj:aes.c
	$(CC) aes.c $(CFLAGS) 

arc4.obj:arc4.c
	$(CC) arc4.c $(CFLAGS) 

base64.obj:base64.c
	$(CC) base64.c $(CFLAGS) 

bignum.obj:bignum.c
	$(CC) bignum.c $(CFLAGS) 

certs.obj:certs.c
	$(CC) certs.c $(CFLAGS) 

debug.obj:debug.c
	$(CC) debug.c $(CFLAGS) 

des.obj:des.c
	$(CC) des.c $(CFLAGS) 

dhm.obj:dhm.c
	$(CC) dhm.c $(CFLAGS) 

havege.obj:havege.c
	$(CC) havege.c $(CFLAGS) 

md2.obj:md2.c
	$(CC) md2.c $(CFLAGS) 

md4.obj:md4.c
	$(CC) md4.c $(CFLAGS) 

md5.obj:md5.c
	$(CC) md5.c $(CFLAGS) 

net.obj:net.c
	$(CC) net.c $(CFLAGS) 

padlock.obj:padlock.c
	$(CC) padlock.c $(CFLAGS) 

rsa.obj:rsa.c
	$(CC) rsa.c $(CFLAGS) 

sha1.obj:sha1.c
	$(CC) sha1.c $(CFLAGS) 

sha2.obj:sha2.c
	$(CC) sha2.c $(CFLAGS) 

sha4.obj:sha4.c
	$(CC) sha4.c $(CFLAGS) 

ssl_cli.obj:ssl_cli.c
	$(CC) ssl_cli.c $(CFLAGS) 

ssl_srv.obj:ssl_srv.c
	$(CC) ssl_srv.c $(CFLAGS) 

ssl_tls.obj:ssl_tls.c
	$(CC) ssl_tls.c $(CFLAGS) 

timing.obj:timing.c
	$(CC) timing.c $(CFLAGS) 

x509parse.obj:x509parse.c
	$(CC) x509parse.c $(CFLAGS) 

xyssl.lib: $(OBJS)
	lib $(OBJS) /OUT:xyssl.lib

clean:
	rm $(OBJS) xyssl.lib
