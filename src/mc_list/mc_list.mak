!include "../windows.mak"

CFLAGS=$(CFLAGSDEBUG)
#CFLAGS=$(CFLAGSRELEASE)

LFLAGS=$(LFLAGSDEBUG)
#LFLAGS=$(LFLAGSRELEASE)	


all:mc_list.lib

list.obj:list.c
	$(CC) list.c $(CFLAGS) 

mc_list.lib: list.obj
	lib list.obj /OUT:mc_list.lib

clean:
	rm list.obj mc_list.lib
