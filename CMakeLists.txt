cmake_minimum_required(VERSION 2.8)

if(CPP_STATIC)
set(CMAKE_CXX_FLAGS "-std=c++14 -m64 -Wall -Wextra -static")
else(CPP_STATIC)
set(CMAKE_CXX_FLAGS "-std=c++14 -m64 -Wall -Wextra")
endif(CPP_STATIC)

#set(CMAKE_CXX_FLAGS_DEBUG  "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG  "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE  "-O3 -DNDEBUG")

INCLUDE_DIRECTORIES(src)
INCLUDE_DIRECTORIES(src/lazy)

#SDSL lib and include directory are here
#Please modify it appropriately.
LINK_DIRECTORIES(~/lib)
include_directories(~/include)


#### Modules
if(NOT USE_OTHER_LIB)
if(NOT MYMOD_DIR)
  set(MYMOD_DIR 1)
  set(MYMOD_SOURCE_DIR ${CMAKE_SOURCE_DIR}/module)
  set(MYMOD_BINARY_DIR ${CMAKE_BINARY_DIR}/module)
  add_subdirectory(${MYMOD_SOURCE_DIR}/libdivsufsort ${MYMOD_BINARY_DIR}/libdivsufsort)
endif()
include_directories(${MYMOD_BINARY_DIR}/libdivsufsort/include)
else()
#libdivsufsort directory is here.
#Please modify it appropriately.
#LINK_DIRECTORIES(./lib)
#INCLUDE_DIRECTORIES(./include)
endif()

# STOOL submodule
FILE(GLOB STOOL_SOURCES module/stool/src/*.cpp)
INCLUDE_DIRECTORIES(module)

# Main programs
FILE(GLOB SOURCES src/*.cpp src/lazy/*.cpp src/greedy/*.cpp)
add_library(fn STATIC ${SOURCES} ${ESAXX_SOURCES} ${STOOL_SOURCES})
target_link_libraries(fn sdsl divsufsort divsufsort64)

add_executable(greedy.out  main/greedy_main.cpp)
add_executable(verify.out main/verify_main.cpp)
add_executable(lazy.out main/lazy_main.cpp)
add_executable(macro.out main/macro_main.cpp)

target_link_libraries(greedy.out fn)
target_link_libraries(verify.out fn)
target_link_libraries(lazy.out fn)
target_link_libraries(macro.out fn)



