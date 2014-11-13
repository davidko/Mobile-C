project(FILE_ATTACHMENT)

include_directories(${LIBMC_SOURCE_DIR}/src/include)
include_directories($ENV{CHHOME}/extern/include)
include_directories(../../../src/include)
#include_directories(/usr/local/ch7.0.0/extern/include)

add_executable(file_server server.c)
add_executable(file_client client.c)

#target_link_libraries(helloworld_server mc)
#target_link_libraries(helloworld_client mc)

if(CMAKE_HOST_WIN32)
target_link_libraries(file_server mc_static ws2_32 embedch chsdk)
target_link_libraries(file_client mc_static ws2_32 embedch chsdk)
else()
target_link_libraries(file_server mc_static embedch chsdk)
target_link_libraries(file_client mc_static embedch chsdk)
endif()


file(GLOB taskFiles "${CMAKE_CURRENT_SOURCE_DIR}/task*.c")
foreach(taskFile ${taskFiles})
    file(COPY ${taskFile} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach()
    
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/data.png" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

