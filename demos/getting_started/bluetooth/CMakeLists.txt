project(BLUETOOTH)

include_directories(${LIBMC_SOURCE_DIR}/src/include)
include_directories($ENV{CHHOME}/extern/include)
include_directories(../../../src/include)
#include_directories(/usr/local/ch7.0.0/extern/include)

add_executable(bluetooth_server server.c)
add_executable(bluetooth_client client.c)

if(CMAKE_HOST_WIN32)
target_link_libraries(bluetooth_server mc_static ws2_32 embedch chsdk)
target_link_libraries(bluetooth_client mc_static ws2_32 embedch chsdk)
else()
target_link_libraries(bluetooth_server mc_static embedch chsdk)
target_link_libraries(bluetooth_client mc_static embedch chsdk)
endif()

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/test1.xml" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
