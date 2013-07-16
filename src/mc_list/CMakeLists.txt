cmake_minimum_required(VERSION 2.6)

project(LIBMC_LIST)

add_library(mc_list SHARED list.c rwlock.c)
add_library(mc_list_static STATIC list.c rwlock.c)
set_target_properties(mc_list_static PROPERTIES COMPILE_FLAGS "-fPIC")
