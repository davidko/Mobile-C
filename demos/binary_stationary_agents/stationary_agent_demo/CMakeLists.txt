project(STATIONARY_AGENT_DEMO)

include_directories(${LIBMC_SOURCE_DIR}/src/include)
include_directories($ENV{CHHOME}/extern/include)
include_directories(../../../src/include)
#include_directories(/usr/local/ch7.0.0/extern/include)

add_executable(stat_agent_demo_server server.c)
add_executable(stat_agent_demo_server2 server2.c)
add_executable(stat_agent_demo_client client.c)


if(CMAKE_HOST_WIN32)
target_link_libraries(stat_agent_demo_server mc_static ws2_32 embedch chsdk)
target_link_libraries(stat_agent_demo_server2 mc_static ws2_32 embedch chsdk)
target_link_libraries(stat_agent_demo_client mc_static ws2_32 embedch chsdk)
else()
target_link_libraries(stat_agent_demo_server mc_static embedch chsdk)
target_link_libraries(stat_agent_demo_server2 mc_static embedch chsdk)
target_link_libraries(stat_agent_demo_client mc_static embedch chsdk)
endif()

file(GLOB xmlFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.xml")
foreach(xmlFile ${xmlFiles})
    file(COPY ${xmlFile} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

