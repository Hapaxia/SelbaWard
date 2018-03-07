cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project (SelbaWard)

set(BUILD_SHARED_LIBS true "Build shared (dynamic) libs")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/SelbaWard)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)
find_package(SFML 2 COMPONENTS system window graphics REQUIRED)

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/SelbaWard/*.cpp)

if(BUILD_SHARED_LIBS)
    set(CMAKE_DEBUG_POSTFIX "_d")
else()
    add_definitions(-DSTATIC_LIB)
    set(CMAKE_DEBUG_POSTFIX "_d_s")
    set(CMAKE_RELEASE_POSTFIX "_s")
endif()

add_library(selbaward ${SOURCES})

set_target_properties(selbaward PROPERTIES CXX_STANDARD 11)
target_link_libraries(selbaward ${SFML_LIBRARIES})
