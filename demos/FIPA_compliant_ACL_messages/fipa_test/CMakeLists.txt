project(FIPA_TEST)

include_directories(${LIBMC_SOURCE_DIR}/src/include)
include_directories($ENV{CHHOME}/extern/include)
include_directories(../../../src/include)
#include_directories(/usr/local/ch7.0.0/extern/include)

add_executable(fipa_server1 server.c)
add_executable(fipa_server2 server2.c)
add_executable(fipa_client client.c)

#target_link_libraries(helloworld_server mc)
#target_link_libraries(helloworld_client mc)

if(CMAKE_HOST_WIN32)
target_link_libraries(fipa_server1 mc_static ws2_32 embedch chsdk)
target_link_libraries(fipa_server2 mc_static ws2_32 embedch chsdk)
target_link_libraries(fipa_client mc_static ws2_32 embedch chsdk)
else()
target_link_libraries(fipa_server1 mc_static embedch chsdk)
target_link_libraries(fipa_server2 mc_static embedch chsdk)
target_link_libraries(fipa_client mc_static embedch chsdk)
endif()


file(GLOB xmlFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.xml")
foreach(xmlFile ${xmlFiles})
    file(COPY ${xmlFile} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/fipa_message.txt" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/http_message.txt" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/demo_exec_time.txt" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
