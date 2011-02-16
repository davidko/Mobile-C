!include "windows.mak"

CFLAGS=$(CFLAGSDEBUG) /IC:\ch\extern\include /Iinclude /I..\ /Imxml-2.2.2 /Ilibb64-1.2\include
#CFLAGS=$(CFLAGSRELEASE) /IC:\ch\extern\include /Iinclude /I..\ /Imxml-2.2.2

LFLAGS=$(LFLAGSDEBUG) /LIBPATH:"C:\Ch\extern\lib" /LIBPATH:"mxml-2.2.2" /LIBPATH:"security\xyssl-0.9\library" /LIBPATH:"mc_list" /LIBPATH:"mc_sync" /LIBPATH:"libb64-1.2\src"
#LFLAGS=$(LFLAGSRELEASE) /LIBPATH:"C:\Ch\extern\lib" /LIBPATH:"mxml-2.2.2" /LIBPATH:"security\xyssl-0.9\library" /LIBPATH:"mc_list" /LIBPATH:"mc_sync" /LIBPATH:"libb64-1.2\src"

LIBS=mc_list.lib mc_sync.lib wsock32.lib mxml.lib embedch.lib libb64.lib

OBJS= acc.obj \
									 agent.obj \
									 agent_return_data.obj \
									 agent_datastate.obj \
									 agent_file_data.obj \
									 agent_mailbox.obj \
									 agent_task.obj \
									 ams.obj \
									 barrier.obj \
									 cmd_prompt.obj \
									 connection.obj \
									 data_structures.obj \
									 df.obj \
									 dynstring.obj \
									 fipa_acl.obj \
									 fipa_envelope.obj \
									 libmc.obj \
									 mc_platform.obj \
									 mc_rwlock.obj \
									 message.obj \
									 message_queue.obj \
									 mtp_http.obj \
									 xml_compose.obj \
									 xml_helper.obj \
									 xml_parser.obj \
									 security/asm.obj \
									 security/asm_node.obj \
									 security/asm_message_parser.obj \
									 security/asm_message_composer.obj \
									 security/mc_dh.obj \
                   security/interface.obj


all:libmc.lib

acc.obj:acc.c 
	$(CC) acc.c $(CFLAGS) 
agent.obj:agent.c 
	$(CC) agent.c $(CFLAGS) 
agent_return_data.obj:agent_return_data.c 
	$(CC) agent_return_data.c $(CFLAGS) 
agent_datastate.obj:agent_datastate.c 
	$(CC) agent_datastate.c $(CFLAGS) 
agent_file_data.obj:agent_file_data.c
	$(CC) agent_file_data.c $(CFLAGS)
agent_mailbox.obj:agent_mailbox.c 
	$(CC) agent_mailbox.c $(CFLAGS) 
agent_task.obj:agent_task.c 
	$(CC) agent_task.c $(CFLAGS) 
ams.obj:ams.c 
	$(CC) ams.c $(CFLAGS) 
barrier.obj:barrier.c 
	$(CC) barrier.c $(CFLAGS) 
cmd_prompt.obj:cmd_prompt.c 
	$(CC) cmd_prompt.c $(CFLAGS) 
connection.obj:connection.c 
	$(CC) connection.c $(CFLAGS) 
data_structures.obj:data_structures.c 
	$(CC) data_structures.c $(CFLAGS) 
df.obj:df.c 
	$(CC) df.c $(CFLAGS) 
dynstring.obj:dynstring.c 
	$(CC) dynstring.c $(CFLAGS) 
fipa_acl.obj:fipa_acl.c 
	$(CC) fipa_acl.c $(CFLAGS) 
fipa_envelope.obj:fipa_envelope.c 
	$(CC) fipa_envelope.c $(CFLAGS) 
libmc.obj:libmc.c 
	$(CC) libmc.c $(CFLAGS) 
mc_platform.obj:mc_platform.c 
	$(CC) mc_platform.c $(CFLAGS) 
mc_rwlock.obj:mc_rwlock.c 
	$(CC) mc_rwlock.c $(CFLAGS) 
message.obj:message.c 
	$(CC) message.c $(CFLAGS) 
message_queue.obj:message_queue.c 
	$(CC) message_queue.c $(CFLAGS) 
mtp_http.obj:mtp_http.c 
	$(CC) mtp_http.c $(CFLAGS) 
xml_compose.obj:xml_compose.c 
	$(CC) xml_compose.c $(CFLAGS) 
xml_helper.obj:xml_helper.c 
	$(CC) xml_helper.c $(CFLAGS) 
xml_parser.obj:xml_parser.c 
	$(CC) xml_parser.c $(CFLAGS) 
security\asm.obj:security\asm.c 
	$(CC) security\asm.c $(CFLAGS) /Fosecurity\asm.obj
security\asm_node.obj:security\asm_node.c 
	$(CC) security\asm_node.c $(CFLAGS) /Fosecurity\asm_node.obj
security\asm_message_parser.obj:security\asm_message_parser.c 
	$(CC) security\asm_message_parser.c $(CFLAGS) /Fosecurity\asm_message_parser.obj
security\asm_message_composer.obj:security\asm_message_composer.c 
	$(CC) security\asm_message_composer.c $(CFLAGS) /Fosecurity\asm_message_composer.obj
security\mc_dh.obj:security\mc_dh.c 
	$(CC) security\mc_dh.c $(CFLAGS) /Fosecurity\mc_dh.obj
security\interface.obj:security\interface.c
	$(CC) security\interface.c $(CFLAGS) /Fosecurity\interface.obj

libmc.lib: $(OBJS)
	lib $(LFLAGS) $(OBJS) $(LIBS) /OUT:libmc.lib

clean:
	rm $(OBJS) libmc.lib
