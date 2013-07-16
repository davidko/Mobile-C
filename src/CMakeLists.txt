
set(PACKAGE_VERSION "@LIBMC_VERSION@")
set(PACKAGE_STRING "MobileC @LIBMC_VERSION@")

configure_file(config.h.in.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)

add_subdirectory(mxml-2.2.2)
add_subdirectory(libb64-1.2)
add_subdirectory(mc_list)
add_subdirectory(mc_sync)

include_directories(${LIBB64_SOURCE_DIR}/include)
include_directories(${MXML_SOURCE_DIR})
include_directories(${LIBMC_LIST_SOURCE_DIR})
include_directories(${LiBMC_SYNC_SOURCE_DIR})
include_directories($ENV{CHHOME}/extern/include)

add_library(mc SHARED
  acc.c
  agent.c
  agent_datastate.c
  agent_file_data.c
  agent_return_data.c
  agent_share_data.c
  agent_task.c
  ams.c
  barrier.c
  cmd_prompt.c
  connection.c
  df.c
  dynstring.c
  fipa_acl.c
  fipa_envelope.c
  libmc.c
  mc_platform.c
  mc_rwlock.c
  message.c
  message_queue.c
  mtp_http.c
  xml_compose.c
  xml_helper.c
  xml_parser.c
)

add_library(mc_static STATIC 
  acc.c
  agent.c
  agent_datastate.c
  agent_file_data.c
  agent_return_data.c
  agent_share_data.c
  agent_task.c
  ams.c
  barrier.c
  cmd_prompt.c
  connection.c
  df.c
  dynstring.c
  fipa_acl.c
  fipa_envelope.c
  libmc.c
  mc_platform.c
  mc_rwlock.c
  message.c
  message_queue.c
  mtp_http.c
  xml_compose.c
  xml_helper.c
  xml_parser.c
)
target_link_libraries(mc mc_list mc_sync b64 mxml)

set_target_properties(mc_static PROPERTIES COMPILE_FLAGS "-fPIC")
target_link_libraries(mc_static mc_list_static mc_sync_static b64_static mxml)
