project(COMPOSE_AGENT_EXAMPLE)

include_directories(${LIBMC_SOURCE_DIR}/src/include)
include_directories($ENV{CHHOME}/extern/include)
include_directories(../../../src/include)
#include_directories(/usr/local/ch7.0.0/extern/include)

add_executable(agent_server server.c)
add_executable(agent_client client.c)


if(CMAKE_HOST_WIN32)
target_link_libraries(agent_server mc_static ws2_32 embedch chsdk)
target_link_libraries(agent_client mc_static ws2_32 embedch chsdk)
else()
target_link_libraries(agent_server mc_static embedch chsdk)
target_link_libraries(agent_client mc_static embedch chsdk)
endif()

file(GLOB xmlFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.xml")
foreach(xmlFile ${xmlFiles})
    file(COPY ${xmlFile} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

