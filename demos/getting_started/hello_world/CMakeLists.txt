project(DEMO_HELLO_WORLD)

include_directories(${LIBMC_SOURCE_DIR}/src/include)
include_directories($ENV{CHHOME}/extern/include)

add_executable(helloworld_server server.c)
add_executable(helloworld_client client.c)

#target_link_libraries(helloworld_server mc)
#target_link_libraries(helloworld_client mc)

target_link_libraries(helloworld_server mc_static ws2_32 embedch chsdk)
target_link_libraries(helloworld_client mc_static ws2_32 embedch chsdk)
