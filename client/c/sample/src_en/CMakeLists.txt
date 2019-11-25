cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

# Project
get_filename_component(PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
set(DEPENDENCIES_DIR ${PROJECT_DIR}/dependencies)
set(PROJECT_NAME sample_en)

# Dependencies
set(CLIENT_DIR ../..)

# Outputs
set(PLATFORM_WINDOWS "Unknown")
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	# 64 bits
	set(PLATFORM_WINDOWS "x64")
endif()

set(OUTPUT_RELEASE lib/${PLATFORM_WINDOWS}//)

project(${PROJECT_NAME} C)

# Define Release by default.
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_RELEASE}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_RELEASE}")
set(CMAKE_EXECUTABLE_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_RELEASE}")

if(NOT TARGET client)
	add_subdirectory("${CLIENT_DIR}" ${CMAKE_BINARY_DIR}/client)
endif()

# Source Files
set(SRC_FILES
    ./sample1.c
    ./sample2.c
    ./sample3.c
    ./sample4.c
    ./sample5.c
)
source_group("Sources" FILES ${SRC_FILES})

# Header Files
set(HEADERS_FILES
)
source_group("Headers" FILES ${HEADERS_FILES})
include_directories(
    ${CLIENT_DIR}/include 
)

# Add library to build.
add_library(${PROJECT_NAME} STATIC
   ${SRC_FILES} ${HEADERS_FILES}
)

# Defines Flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Preprocessor definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE 
	-D_DEBUG
	-D_LIB
	-Dsample1=sample1_en
	-Dsample2=sample2_en
	-Dsample3=sample3_en
	-Dsample4=sample4_en
	-Dsample5=sample5_en
)

target_compile_options(${PROJECT_NAME} PRIVATE  /W3 /Zi /EHsc)

# Project dependencies
add_dependencies(${PROJECT_NAME} client)

# Link with other dependencies.
target_link_libraries(${PROJECT_NAME} client)