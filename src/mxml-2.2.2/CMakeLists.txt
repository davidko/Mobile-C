cmake_minimum_required(VERSION 2.8)

project (MXML)

include (FindPkgConfig)
include (CheckIncludeFiles)
include (CheckFunctionExists)
include (CheckTypeSize)
include (CheckLibraryExists)

if(CMAKE_HOST_WIN32)
else(CMAKE_HOST_WIN32)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
endif(CMAKE_HOST_WIN32)

set(CMAKE_EXTRA_INCLUDE_FILES stdio.h)
CHECK_TYPE_SIZE("long long" LONG_LONG)
set(CMAKE_EXTRA_INCLUDE_FILES)
CHECK_FUNCTION_EXISTS(strdup HAVE_STRDUP)
CHECK_FUNCTION_EXISTS(snprintf HAVE_SNPRINTF)
CHECK_FUNCTION_EXISTS(vsnprintf HAVE_VSNPRINTF)
CHECK_INCLUDE_FILES(pthread.h HAVE_PTHREAD_H)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

add_library(mxml STATIC 
    mxml-attr.c
    mxmldoc.c
    mxml-entity.c
    mxml-file.c
#mxml-get.c
    mxml-index.c
    mxml-node.c
    mxml-private.c
    mxml-search.c
    mxml-set.c
    mxml-string.c)

if(CMAKE_HOST_WIN32)
if(CMAKE_GENERATOR MATCHES "MSYS")
target_link_libraries(mxml pthread)
else()
target_link_libraries(mxml)
endif()
else(CMAKE_HOST_WIN32)
target_link_libraries(mxml pthread)
endif(CMAKE_HOST_WIN32)

