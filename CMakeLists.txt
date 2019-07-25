cmake_minimum_required(VERSION 2.8)


if(APPLE)
set(CMAKE_CXX_FLAGS "-std=c++14  -m64  -D_INT128")
else(APPLE)
set(CMAKE_CXX_FLAGS "-std=c++14  -m64  -D_INT128 -static")
endif(APPLE)

#set(CMAKE_CXX_FLAGS_DEBUG  "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG  "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE  "-O3 -DNDEBUG")

INCLUDE_DIRECTORIES(src)
INCLUDE_DIRECTORIES(src/lazy)
INCLUDE_DIRECTORIES(src/ms)
INCLUDE_DIRECTORIES(src/ms/fast)
FILE(GLOB SOURCES src/*.cpp src/lazy/*.cpp src/ms/*.cpp src/ms/fast/*.cpp)
#set(EXECUTABLE_OUTPUT_PATH ./bin)


#### Modules
if(NOT MYMOD_DIR)
  set(MYMOD_DIR 1)
  set(MYMOD_SOURCE_DIR ${CMAKE_SOURCE_DIR}/module)
  set(MYMOD_BINARY_DIR ${CMAKE_BINARY_DIR}/module)
  add_subdirectory(${MYMOD_SOURCE_DIR}/libdivsufsort ${MYMOD_BINARY_DIR}/libdivsufsort)
endif()
include_directories(${MYMOD_BINARY_DIR}/libdivsufsort/include)

#SDSL lib and include directory are here
LINK_DIRECTORIES(~/lib)
include_directories(~/include)



add_library(ms STATIC module/esaxx/minimal_subsrings/minimal_substring_tree.hpp)
#target_link_libraries(fn sdsl divsufsort divsufsort64)


add_library(fn STATIC ${SOURCES})
target_link_libraries(fn sdsl divsufsort divsufsort64)


#add_executable(a.out main.cpp)
add_executable(msubstr.out msubstr_main.cpp)
add_executable(greedy.out greedy_main.cpp)
add_executable(verify.out verify_main.cpp)
#add_executable(converter.out converter_main.cpp)
add_executable(lazy.out lazy_main.cpp)


#target_link_libraries(a.out fn)
target_link_libraries(msubstr.out fn ms)
target_link_libraries(greedy.out fn)
target_link_libraries(verify.out fn)
#target_link_libraries(converter.out fn)
target_link_libraries(lazy.out fn)



