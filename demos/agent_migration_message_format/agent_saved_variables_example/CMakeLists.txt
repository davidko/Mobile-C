project(DEMO_DATASTATE_EXAMPLE)

include_directories(${LIBMC_SOURCE_DIR}/src/include)
include_directories($ENV{CHHOME}/extern/include)
include_directories(../../../src/include)
#include_directories(/usr/local/ch7.0.0/extern/include)

add_executable(savedvariables_server server.c)
add_executable(savedvariables_client client.c)

#target_link_libraries(helloworld_server mc)
#target_link_libraries(helloworld_client mc)

if(CMAKE_HOST_WIN32)
target_link_libraries(savedvariables_server mc_static ws2_32 embedch chsdk)
target_link_libraries(savedvariables_client mc_static ws2_32 embedch chsdk)
else()
target_link_libraries(savedvariables_server mc_static embedch chsdk)
target_link_libraries(savedvariables_client mc_static embedch chsdk)
endif()

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/test1.xml" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
