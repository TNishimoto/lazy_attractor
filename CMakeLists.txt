cmake_minimum_required(VERSION 2.8)
project(lazy_attractor CXX)

if(CPP_STATIC)
set(CMAKE_CXX_FLAGS "-std=c++14 -m64 -Wall -Wextra -static")
else(CPP_STATIC)
set(CMAKE_CXX_FLAGS "-std=c++14 -m64 -Wall -Wextra")
endif(CPP_STATIC)

#set(CMAKE_CXX_FLAGS_DEBUG  "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG  "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE  "-O3 -DNDEBUG")


#set(ROOT_PROJECT  "lazy_attractor")
if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    message("Release Mode")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

# Set PROJECT_IS_TOP_LEVEL to ON if if this is the top level project (not
# if this is included as a subproject of something else). Note that this is
# handled automatically for CMake >= 3.21.
if (CMAKE_VERSION VERSION_LESS 3.21)
    if ("${CMAKE_PROJECT_NAME}" STREQUAL "${PROJECT_NAME}")
        message("SET PROJECT_IS_TOP_LEVEL ON IN ${PROJECT_NAME}")
        set (PROJECT_IS_TOP_LEVEL ON)
    else()
        message("SET PROJECT_IS_TOP_LEVEL OFF IN ${PROJECT_NAME}")
    set (PROJECT_IS_TOP_LEVEL OFF)    
    endif()
endif ()


INCLUDE_DIRECTORIES(include)
#INCLUDE_DIRECTORIES(src/lazy)

#SDSL lib and include directory are here
#Please modify it appropriately.
LINK_DIRECTORIES(~/lib)
include_directories(~/include)

if(NOT TARGET stool)
add_subdirectory(module/stool)
endif()

if(NOT TARGET esaxx)
add_subdirectory(module/esaxx)
endif()



#### Modules
#if(NOT USE_OTHER_LIB)
#if(NOT MYMOD_DIR)
#  set(MYMOD_DIR 1)
#  set(MYMOD_SOURCE_DIR ${CMAKE_SOURCE_DIR}/module)
#  set(MYMOD_BINARY_DIR ${CMAKE_BINARY_DIR}/module)
#  add_subdirectory(${MYMOD_SOURCE_DIR}/libdivsufsort ${MYMOD_BINARY_DIR}/libdivsufsort)
#endif()
#include_directories(${MYMOD_BINARY_DIR}/libdivsufsort/include)
#else()
##libdivsufsort directory is here.
##Please modify it appropriately.
##LINK_DIRECTORIES(./lib)
##INCLUDE_DIRECTORIES(./include)
#endif()

# STOOL submodule
#FILE(GLOB STOOL_SOURCES module/stool/lib/*.cpp)
#INCLUDE_DIRECTORIES(module)

# ListOrderMaintenance submodule
FILE(GLOB LOM_SOURCES module/list_order_maintenance/lib/*.cpp)
INCLUDE_DIRECTORIES(module)


# Main programs
FILE(GLOB SOURCES lib/*.cpp lib/lazy/*.cpp lib/greedy/*.cpp)



#add_executable(macro.out src/main/macro_main.cpp)
#target_link_libraries(macro.out fn)

if (PROJECT_IS_TOP_LEVEL)
message("TOP LEVEL IS ${PROJECT_NAME}")

add_library(fn STATIC ${SOURCES} ${LOM_SOURCES} module/libdivsufsort/sa.cpp)
target_link_libraries(fn sdsl divsufsort divsufsort64 stool)

add_executable(lazy.out main/lazy_main.cpp)
add_executable(verify.out main/verify_main.cpp)
add_executable(print.out main/print_main.cpp)
add_executable(demo.out main/demo_main.cpp)
#add_executable(greedy.out  main/greedy_main.cpp)

#add_executable(test.out main/test/test_main.cpp)
add_executable(exact_2_attrs.out main/exact_2_attrs_main.cpp)
add_executable(appro_2_attrs.out main/appro_2_attrs_main.cpp)

target_link_libraries(lazy.out fn)
target_link_libraries(verify.out fn)
target_link_libraries(print.out fn)
target_link_libraries(demo.out fn)
#target_link_libraries(greedy.out fn)

#target_link_libraries(test.out fn)
target_link_libraries(exact_2_attrs.out fn)
target_link_libraries(appro_2_attrs.out fn)

else()
endif()



