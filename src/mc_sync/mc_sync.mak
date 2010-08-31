!include "../windows.mak"

CFLAGS=$(CFLAGSDEBUG)
#CFLAGS=$(CFLAGSRELEASE)

LFLAGS=$(LFLAGSDEBUG)
#LFLAGS=$(LFLAGSRELEASE)	


all:mc_sync.lib

sync_list.obj:sync_list.c
	$(CC) sync_list.c $(CFLAGS) 

mc_sync.lib: sync_list.obj 
	lib sync_list.obj /OUT:mc_sync.lib

clean:
	rm sync_list.obj mc_sync.lib
