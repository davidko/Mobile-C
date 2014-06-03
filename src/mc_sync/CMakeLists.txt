cmake_minimum_required(VERSION 2.6)

project(LIBMC_SYNC)

add_library(mc_sync SHARED sync_list.c)
add_library(mc_sync_static STATIC sync_list.c)
set_target_properties(mc_sync_static PROPERTIES COMPILE_FLAGS "-fPIC")

link_directories(${LIBMC_LIST_BINARY_DIR})
target_link_libraries(mc_sync mc_list_static)
