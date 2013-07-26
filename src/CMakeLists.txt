
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

link_directories($ENV{CHHOME}/extern/lib)

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
target_link_libraries(mc mc_list_static mc_sync_static b64_static mxml -Wl,--whole-archive -lembedch -lchsdk -Wl,--no-whole-archive)
set_target_properties(mc PROPERTIES COMPILE_FLAGS "-pthread")
set_target_properties(mc PROPERTIES LINK_FLAGS "-pthread -rdynamic")

set_target_properties(mc_static PROPERTIES COMPILE_FLAGS "-pthread -fPIC")
set_target_properties(mc_static PROPERTIES LINK_FLAGS "-pthread -rdynamic")
target_link_libraries(mc_static mc_list_static mc_sync_static b64_static mxml embedch)

install(TARGETS mc DESTINATION lib)


if(CMAKE_SYSTEM_NAME MATCHES [Ll]inux)
  string(TOLOWER "${CPACK_PACKAGE_NAME}" CPACK_PACKAGE_NAME_LOWERCASE)
  find_program(DPKG_PROGRAM dpkg DOC "dpkg program of Debian-based systems")
  if(DPKG_PROGRAM)
       execute_process(COMMAND ${DPKG_PROGRAM} --print-architecture
                 OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE
                 OUTPUT_STRIP_TRAILING_WHITESPACE)
       set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME_LOWERCASE}_${PROJECT_VERSION}-${CPACK_DEBIAN_PACKAGE_REVISION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}") 
  else(DPKG_PROGRAM)
       set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME_LOWERCASE}_${PROJECT_VERSION}-${CPACK_DEBIAN_PACKAGE_REVISION}_${CMAKE_SYSTEM_NAME}")
  endif(DPKG_PROGRAM)

  INCLUDE (InstallRequiredSystemLibraries)

  SET (CPACK_SET_DESTDIR "on")
  SET (CPACK_PACKAGE_NAME "libmc")
  SET (CPACK_PACKAGING_INSTALL_PREFIX "/tmp")
  SET (CPACK_GENERATOR "DEB")

  SET (VERSION 1.2.0)
  SET (CPACK_DEBIAN_PACKAGE_VERSION ${VERSION})
  SET (CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
  SET (CPACK_DEBIAN_PACKAGE_SECTION "Science & Engineering")
  SET (CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
#SET (CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libgladeui-1-11, libglib2.0-0, libgtk2.0-0")
  set (CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

  SET (CPACK_PACKAGE_DESCRIPTION "Mobile-C Mobile Agents")
  SET (CPACK_PACKAGE_DESCRIPTION_SUMMARY "A C library for creating Mobile-C mobile agents")
  SET (CPACK_PACKAGE_CONTACT "david.dk.ko@gmail.com")
  SET (CPACK_DEBIAN_PACKAGE_MAINTAINER "David Ko <david.dk.ko@gmail.com>")
  SET (CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")

  SET (CPACK_COMPONENTS_ALL Libraries ApplicationData)
  INCLUDE (CPack)
endif()
