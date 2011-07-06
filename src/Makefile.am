AUTOMAKE_OPTIONS = gnu
SUBDIRS = mc_sync mc_list mxml-2.2.2 security/xyssl-0.9/library libb64-1.2/src
lib_LTLIBRARIES = libmc.la
libmc_la_SOURCES = acc.c \
									 agent.c \
									 agent_file_data.c \
									 agent_return_data.c \
									 agent_datastate.c \
									 agent_task.c \
									 ams.c \
									 barrier.c \
									 cmd_prompt.c \
									 connection.c \
									 df.c \
									 dynstring.c \
									 fipa_acl.c \
									 fipa_envelope.c \
									 libmc.c \
									 mc_platform.c \
									 mc_rwlock.c \
									 message.c \
									 message_queue.c \
									 mtp_http.c \
									 xml_compose.c \
									 xml_helper.c \
									 xml_parser.c \
									 security/asm.c \
									 security/asm_node.c \
									 security/asm_message_parser.c \
									 security/asm_message_composer.c \
									 security/mc_dh.c \
                                                                         security/interface.c

libmc_la_LIBADD = mc_sync/libmc_sync.la mc_list/libmc_list.la security/xyssl-0.9/library/libxyssl.la -Lmxml-2.2.2 -lmxml -Llibb64-1.2/src -lb64
libmc_la_LDFLAGS = -rdynamic
INCLUDES = -Imc_sync -Imc_list -Imxml-2.2.2 -Isecurity/xyssl-0.9/include -Ilibb64-1.2/include

stuffdir = $(includedir)
dist_stuff_HEADERS = include/libmc.h include/fipa_acl.h include/dynstring.h
noinst_HEADERS = include/ams.h \
								 include/agent_datastate.h \
								 include/agent_task.h \
								 include/barrier.h \
								 include/cmd_prompt.h \
								 include/commands.h \
								 include/commands.x.h \
								 include/df.h \
								 include/df_request.x.h \
								 include/fipa_acl_envelope.h \
								 include/interpreter_variable_data.h \
								 include/libmc.h \
								 include/macros.h \
								 include/mc_error.h \
								 include/mc_platform.h \
								 include/mc_rwlock.h \
								 include/mtp_http.h \
								 include/xml_compose.h \
								 include/xml_helper.h \
								 include/xml_parser.h \
								 security/asm.h \
								 security/asm_message_parser.h \
								 security/asm_message_composer.h \
								 security/asm_node.h \
								 security/mc_dh.h \
                                                                 security/interface.h 

								 libmc_la_LDFLAGS = -version-info 1:0:0 -no-undefined
								 libmc_la_CFLAGS = -D_MC_LIB 
