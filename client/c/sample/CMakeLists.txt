cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

# Project
get_filename_component(PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
set(DEPENDENCIES_DIR ${PROJECT_DIR}/dependencies)
set(PROJECT_NAME sample)
set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Dependencies
set(SAMPLE_EN_DIR src_en)
set(SAMPLE_JA_DIR src_ja)

project(${PROJECT_NAME} C)

# Define Release by default.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

if(NOT TARGET sample_en)
	add_subdirectory("${SAMPLE_EN_DIR}" ${CMAKE_BINARY_DIR}/sample_en)
endif()
if(NOT TARGET sample_ja)
	add_subdirectory("${SAMPLE_JA_DIR}" ${CMAKE_BINARY_DIR}/sample_ja)
endif()

# Source Files
set(SRC_FILES
    ./main.c
)
source_group("Sources" FILES ${SRC_FILES})

# Header Files
set(HEADERS_FILES
    client_sample.h
)
source_group("Headers" FILES ${HEADERS_FILES})

# Outputs
set(PLATFORM_WINDOWS "Unknown")
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	# 64 bits
	set(PLATFORM_WINDOWS "x64")
endif()

set(OUTPUT_RELEASE ../../../bin/${PLATFORM_WINDOWS}//)

# Defines outputs
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_RELEASE}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_RELEASE}")
set(CMAKE_EXECUTABLE_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_RELEASE}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_RELEASE}")
set(CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_RELEASE}")

# Add executable to build.
add_executable(${PROJECT_NAME} 
   ${SRC_FILES} ${HEADERS_FILES}
)

# Defines Flags for Windows
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Preprocessor definitions
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/LTCG")
target_compile_definitions(${PROJECT_NAME} PRIVATE 
   -DNDEBUG
   -D_WINDOWS
   -D_CONSOLE 
)

target_compile_options(${PROJECT_NAME} PRIVATE  /W3 /Zi /EHsc)

# Project dependencies
add_dependencies(${PROJECT_NAME} sample_en sample_ja)

# Link with other dependencies.
target_link_libraries(${PROJECT_NAME} sample_en sample_ja)