cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project (SelbaWard)

set(BUILD_SHARED_LIBS ON "Build shared (dynamic) libs")
set(BUILD_EXAMPLES OFF "Build the examples")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/SelbaWard)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
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

install(
	DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
	DESTINATION .
	FILES_MATCHING PATTERN "*.hpp" PATTERN "*.inl")

install(
	DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include
	DESTINATION .
	FILES_MATCHING PATTERN "*.hpp" PATTERN "*.inl")

install(
        TARGETS selbaward
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

if(BUILD_EXAMPLES)
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/examples/resources DESTINATION examples)
	file(GLOB EXAMPLES_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/examples/*.cpp)
	foreach(EXAMPLES_SOURCE ${EXAMPLES_SOURCES})
		get_filename_component(EXAMPLES_SOURCE_BASE_NAME ${EXAMPLES_SOURCE} NAME_WE)
		add_executable(${EXAMPLES_SOURCE_BASE_NAME} ${EXAMPLES_SOURCE})
		set_target_properties(${EXAMPLES_SOURCE_BASE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY examples)
		target_link_libraries(${EXAMPLES_SOURCE_BASE_NAME} ${SFML_LIBRARIES})
		link_directories(${CMAKE_BINARY_DIR})
		target_link_libraries(${EXAMPLES_SOURCE_BASE_NAME} selbaward)
	endforeach(EXAMPLES_SOURCE)
endif()
