cmake_minimum_required(VERSION 2.6)

project(LIBMC_LIST)

add_library(mc_list SHARED list.c)
add_library(mc_list_static STATIC list.c)
