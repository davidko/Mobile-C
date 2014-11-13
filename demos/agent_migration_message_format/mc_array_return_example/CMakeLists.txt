project(MC_ARRAY_RETURN_EXAMPLE)

include_directories(${LIBMC_SOURCE_DIR}/src/include)
include_directories($ENV{CHHOME}/extern/include)
include_directories(../../../src/include)
#include_directories(/usr/local/ch7.0.0/extern/include)

add_executable(arrayreturn_server server.c)
add_executable(arrayreturn_client client.c)

if(CMAKE_HOST_WIN32)
target_link_libraries(arrayreturn_server mc_static ws2_32 embedch chsdk)
target_link_libraries(arrayreturn_client mc_static ws2_32 embedch chsdk)
else()
target_link_libraries(arrayreturn_server mc_static embedch chsdk)
target_link_libraries(arrayreturn_client mc_static embedch chsdk)
endif()
