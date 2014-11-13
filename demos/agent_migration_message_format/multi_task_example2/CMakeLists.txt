project(MC_MULTI_TASK_EXAMPLE)

include_directories(${LIBMC_SOURCE_DIR}/src/include)
include_directories($ENV{CHHOME}/extern/include)
include_directories(../../../src/include)
#include_directories(/usr/local/ch7.0.0/extern/include)

add_executable(multitask_server1 server1.c)
add_executable(multitask_server2 server2.c)
add_executable(multitask_client client.c)

if(CMAKE_HOST_WIN32)
target_link_libraries(multitask_server1 mc_static ws2_32 embedch chsdk)
target_link_libraries(multitask_server2 mc_static ws2_32 embedch chsdk)
target_link_libraries(multitask_client mc_static ws2_32 embedch chsdk)
else()
target_link_libraries(multitask_server1 mc_static embedch chsdk)
target_link_libraries(multitask_server2 mc_static embedch chsdk)
target_link_libraries(multitask_client mc_static embedch chsdk)
endif()

file(GLOB xmlFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.xml")
foreach(xmlFile ${xmlFiles})
    file(COPY ${xmlFile} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach()
    
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/ChDataFile_ch" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/ChDataFile_iel2" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
