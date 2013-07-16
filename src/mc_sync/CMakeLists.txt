cmake_minimum_required(VERSION 2.6)

project(LIBMC_SYNC)

add_library(mc_sync SHARED sync_list.c)
add_library(mc_sync_static STATIC sync_list.c)
