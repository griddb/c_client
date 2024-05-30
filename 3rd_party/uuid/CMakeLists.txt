cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

# Project
get_filename_component(PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
set(DEPENDENCIES_DIR ${PROJECT_DIR}/dependencies)
set(PROJECT_NAME uuid)
add_definitions(/Zi)

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

# Defines outputs
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_RELEASE}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_RELEASE}")
set(CMAKE_EXECUTABLE_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_RELEASE}")

# Source Files
set(SRC_FILES
    uuid/clear.c
    uuid/compare.c
    uuid/copy.c
    uuid/gen_uuid_nt2.c
    uuid/isnull.c
    uuid/pack.c
    uuid/parse.c
    uuid/unpack.c
    uuid/uuid_time.c
)
source_group("Sources" FILES ${SRC_FILES})

# Header Files
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(HEADERS_FILES
    uuid/config.h
    uuid/uuid.h
    uuid/uuidd.h
    uuid/uuidP.h
    uuid/uuid_types.h

)
source_group("Headers" FILES ${HEADERS_FILES})

# Add library to build.
add_library(${PROJECT_NAME} STATIC
   ${SRC_FILES} ${HEADERS_FILES}
)

# Defines Flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

target_compile_definitions(${PROJECT_NAME} PRIVATE
	-DNDEBUG
	-D_LIB
	-D_SECURE_SCL=0
)

# Preprocessor definitions
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/GL")
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/LTCG")
target_compile_options(${PROJECT_NAME} PRIVATE  /W3 /EHsc)

target_include_directories(${PROJECT_NAME} PUBLIC ${HEADERS_FILES} .)