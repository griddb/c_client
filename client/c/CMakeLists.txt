cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

# Project
get_filename_component(PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
set(DEPENDENCIES_DIR ${PROJECT_DIR}/dependencies)
set(PROJECT_NAME client)
set(OUTPUT_FILE_NAME gridstore_c)
set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Dependencies
set(UUID_DIR ../../3rd_party/uuid)
set(UTILITY_DIR ../../utility)
set(C_CLIENT_DIR ../../)

# Outputs
set(PLATFORM_WINDOWS "Unknown")
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	# 64 bits
	set(PLATFORM_WINDOWS "x64")
endif()

set(OUTPUT_RELEASE ../../bin/${PLATFORM_WINDOWS}//)

project(${PROJECT_NAME} C)

# Define Release by default.
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

# Defines outputs , depending BUILD TYPE
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_RELEASE}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_RELEASE}")
set(CMAKE_EXECUTABLE_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_RELEASE}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_RELEASE}")
set(CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_RELEASE}")

if(NOT TARGET uuid)
	add_subdirectory("${UUID_DIR}" ${CMAKE_BINARY_DIR}/uuid)
endif()
if(NOT TARGET utility)
	add_subdirectory("${UTILITY_DIR}" ${CMAKE_BINARY_DIR}/utility)
endif()

# Files of project and target to build
# Source Files
set(SRC_FILES
    ./client.cpp
    ./container_key_utils.cpp
	${C_CLIENT_DIR}/server/service_address.cpp
	${C_CLIENT_DIR}/server/authentication.cpp
	${C_CLIENT_DIR}/3rd_party/omaha/omaha/base/security/md5.c
	${C_CLIENT_DIR}/3rd_party/sha2/sha2.c
	${C_CLIENT_DIR}/server/container_key.cpp
	${C_CLIENT_DIR}/3rd_party/ebb/ebb_request_parser.c
	${C_CLIENT_DIR}/server/http.cpp
	${C_CLIENT_DIR}/server/json.cpp
	${C_CLIENT_DIR}/server/uuid_utils.cpp
	${C_CLIENT_DIR}/server/bit_array.cpp
	${C_CLIENT_DIR}/server/socket_wrapper.cpp
)
source_group("Sources" FILES ${SRC_FILES})

# Header Files
include_directories(
    ${C_CLIENT_DIR}/client/c/include 
    ${C_CLIENT_DIR}/client/c/ext/include
    ${UTILITY_DIR}/
    ${C_CLIENT_DIR}/client/c/ 
    ${C_CLIENT_DIR}/server/
    ${C_CLIENT_DIR}/3rd_party/sha2 
    ${C_CLIENT_DIR}/3rd_party/omaha/omaha/base/security 
    ${C_CLIENT_DIR}/3rd_party/picojson/include/
    ${C_CLIENT_DIR}/3rd_party/ebb/
)

set(HEADERS_FILES
    ./client.h
	${C_CLIENT_DIR}/utility/util/container.h
	${C_CLIENT_DIR}/server/gs_error_common.h
	${C_CLIENT_DIR}/3rd_party/sha2/sha2.h
	${C_CLIENT_DIR}/3rd_party/omaha/omaha/base/security/md5.h
	${C_CLIENT_DIR}/client/c/include/gridstore.h
	${C_CLIENT_DIR}/server/authentication.h
	${C_CLIENT_DIR}/server/service_address.h
	${C_CLIENT_DIR}/server/socket_wrapper.h
)
source_group("Headers" FILES ${HEADERS_FILES})

# Add library to build.
add_library(${PROJECT_NAME} SHARED
   ${SRC_FILES} ${HEADERS_FILES}
)

# Defines Flags for Windows
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Preprocessor definitions
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/LTCG")
target_compile_definitions(${PROJECT_NAME} PRIVATE 
   -DNDEBUG
   -D_WINDOWS
   -D_USRDLL
   -D_SECURE_SCL=0
   -DGS_DLL_INSIDE
   -DGS_EXPERIMENTAL_TOOL_ENABLED=1
   -DGS_ERROR_COMMON_ONLY=1 
)

target_compile_options(${PROJECT_NAME} PRIVATE  /W3 /Zi /EHsc)

# Add project dependencies and Link to project
add_dependencies(${PROJECT_NAME} utility uuid )

# Link with other dependencies.
target_link_libraries(${PROJECT_NAME} utility uuid ws2_32)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${OUTPUT_FILE_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${HEADERS_FILES})