project(FIPA_COMM)

include_directories(${LIBMC_SOURCE_DIR}/src/include)
include_directories($ENV{CHHOME}/extern/include)
include_directories(../../../src/include)
#include_directories(/usr/local/ch7.0.0/extern/include)

add_executable(fipa_server server.c)

if(CMAKE_HOST_WIN32)
target_link_libraries(fipa_server mc_static ws2_32 embedch chsdk)
else()
target_link_libraries(fipa_server mc_static embedch chsdk)
endif()

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/agents/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/agents")

