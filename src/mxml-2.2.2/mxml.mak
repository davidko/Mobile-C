!include "../windows.mak"

CFLAGS=$(CFLAGSDEBUG) 
#CFLAGS=$(CFLAGSRELEASE) 

all:mxml.lib

OBJS= \
mxml-attr.obj \
mxml-entity.obj \
mxml-file.obj \
mxml-index.obj \
mxml-node.obj \
mxml-private.obj \
mxml-search.obj \
mxml-set.obj \
mxml-string.obj

mxml-attr.obj:mxml-attr.c
	$(CC) mxml-attr.c $(CFLAGS) 
mxml-entity.obj:mxml-entity.c
	$(CC) mxml-entity.c $(CFLAGS) 
mxml-file.obj:mxml-file.c
	$(CC) mxml-file.c $(CFLAGS) 
mxml-index.obj:mxml-index.c
	$(CC) mxml-index.c $(CFLAGS) 
mxml-node.obj:mxml-node.c
	$(CC) mxml-node.c $(CFLAGS) 
mxml-private.obj:mxml-private.c
	$(CC) mxml-private.c $(CFLAGS) 
mxml-search.obj:mxml-search.c
	$(CC) mxml-search.c $(CFLAGS) 
mxml-set.obj:mxml-set.c
	$(CC) mxml-set.c $(CFLAGS) 
mxml-string.obj:mxml-string.c
	$(CC) mxml-string.c $(CFLAGS) 

mxml.lib: $(OBJS)
  lib $(OBJS) /OUT:mxml.lib

clean:
	rm $(OBJS) mxml.lib
