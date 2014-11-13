project(SQUAD_TEST)

include_directories(${LIBMC_SOURCE_DIR}/src/include)
include_directories($ENV{CHHOME}/extern/include)
include_directories(../../../src/include)
#include_directories(/usr/local/ch7.0.0/extern/include)

add_executable(squad_server1 server1.c)
add_executable(squad_server2 server2.c)
add_executable(squad_server3 server3.c)
add_executable(squad_client client.c)

#target_link_libraries(helloworld_server mc)
#target_link_libraries(helloworld_client mc)

if(CMAKE_HOST_WIN32)
target_link_libraries(squad_server1 mc_static ws2_32 embedch chsdk)
target_link_libraries(squad_server2 mc_static ws2_32 embedch chsdk)
target_link_libraries(squad_server3 mc_static ws2_32 embedch chsdk)
target_link_libraries(squad_client mc_static ws2_32 embedch chsdk)
else()
target_link_libraries(squad_server1 mc_static embedch chsdk)
target_link_libraries(squad_server2 mc_static embedch chsdk)
target_link_libraries(squad_server3 mc_static embedch chsdk)
target_link_libraries(squad_client mc_static embedch chsdk)
endif()


file(GLOB taskFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.xml")
foreach(taskFile ${taskFiles})
    file(COPY ${taskFile} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach()
    

