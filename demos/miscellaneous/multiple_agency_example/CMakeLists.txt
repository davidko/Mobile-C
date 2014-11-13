project(MULTIPLE_AGENCY)

include_directories(${LIBMC_SOURCE_DIR}/src/include)
include_directories($ENV{CHHOME}/extern/include)
include_directories(../../../src/include)
#include_directories(/usr/local/ch7.0.0/extern/include)

add_executable(multiple_agency_server server.c)
add_executable(multiple_agency_client client.c)

#target_link_libraries(helloworld_server mc)
#target_link_libraries(helloworld_client mc)

if(CMAKE_HOST_WIN32)
target_link_libraries(multiple_agency_server mc_static ws2_32 embedch chsdk)
target_link_libraries(multiple_agency_client mc_static ws2_32 embedch chsdk)
else()
target_link_libraries(multiple_agency_server mc_static embedch chsdk)
target_link_libraries(multiple_agency_client mc_static embedch chsdk)
endif()


file(GLOB taskFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.xml")
foreach(taskFile ${taskFiles})
    file(COPY ${taskFile} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach()
    

