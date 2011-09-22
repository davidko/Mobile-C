!include "../windows.mak"

CFLAGS=$(CFLAGSDEBUG)
#CFLAGS=$(CFLAGSRELEASE)

LFLAGS=$(LFLAGSDEBUG)
#LFLAGS=$(LFLAGSRELEASE)	


all:mc_list.lib

list.obj:list.c
	$(CC) list.c $(CFLAGS) 
rwlock.obj:rwlock.c
	$(CC) rwlock.c $(CFLAGS)

mc_list.lib: list.obj rwlock.obj
	lib list.obj rwlock.obj /OUT:mc_list.lib

clean:
	rm list.obj rwlock.obj mc_list.lib
