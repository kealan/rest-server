cmake_minimum_required (VERSION 3.1)
project (REST-SERVER)

# enable testing
enable_testing ()

# Extract version from the VERSION file
file(STRINGS VERSION VERSION_FILE_CONTENT)
string(REPLACE "." ";" VERSION_FILE_PARTS ${VERSION_FILE_CONTENT})
list(GET VERSION_FILE_PARTS 0 AMCL_VERSION_MAJOR)
list(GET VERSION_FILE_PARTS 1 AMCL_VERSION_MINOR)
list(GET VERSION_FILE_PARTS 2 AMCL_VERSION_PATCH)
set(PROJECT_VERSION "${AMCL_VERSION_MAJOR}.${AMCL_VERSION_MINOR}.${AMCL_VERSION_PATCH}")
message(STATUS "PROJECT_VERSION='${PROJECT_VERSION}'")
message(STATUS "Generator - ${CMAKE_GENERATOR}")

# Add options for build
option (BUILD_DOXYGEN "Build Doxygen" OFF)

# Configure build
set(VERSION ${PROJECT_VERSION} CACHE STRING "Build version")
set(SERVICE "test" CACHE STRING "Service name")
message(STATUS "VERSION=${VERSION}")
message(STATUS "SERVICE=${SERVICE}")
# Configure test variables
configure_file (
  "${PROJECT_SOURCE_DIR}/include/config.h.in"
  "${PROJECT_BINARY_DIR}/include/config.h"
)

set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE}
    CACHE STRING "Choose the type of build: Debug Release Coverage ASan Check CheckFull"
    FORCE)

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

if(CMAKE_COMPILER_IS_GNUCC)
    message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
    execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
                    OUTPUT_VARIABLE GCC_VERSION)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static  -Wall -Wextra -Wno-strict-prototypes -Wunused-value -Wcast-align -Wunused-variable -Wundef -Wformat-security")

    if (GCC_VERSION VERSION_GREATER 4.8 OR GCC_VERSION VERSION_EQUAL 4.8)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wshadow")
        set(CM./build -s ReleaseAKE_C_FLAGS_ASAN    "-O0 -g3 -fsanitize=address")
    else (GCC_VERSION VERSION_GREATER 4.8 OR GCC_VERSION VERSION_EQUAL 4.8)
        message(STATUS "GCC 4.8 required to run address sanitizer - please upgrade your installation")
    endif(GCC_VERSION VERSION_GREATER 4.8 OR GCC_VERSION VERSION_EQUAL 4.8)

    set(CMAKE_C_FLAGS_RELEASE     "-O2")
    set(CMAKE_C_FLAGS_DEBUG       "-O0 -g3 -D DEBUG")
    set(CMAKE_C_FLAGS_COVERAGE    "-O0 -g3 --coverage")
    set(CMAKE_C_FLAGS_CHECK       "-O2 -Werror")
    set(CMAKE_C_FLAGS_CHECKFULL   "${CMAKE_C_FLAGS_CHECK} -Wcast-qual")
endif(CMAKE_COMPILER_IS_GNUCC)

if(CMAKE_BUILD_TYPE STREQUAL "Coverage")
    set(CMAKE_SHARED_LINKER_FLAGS "--coverage")
endif(CMAKE_BUILD_TYPE STREQUAL "Coverage")

# Add subdirectories
add_subdirectory(src)

# Build Documentation
if (BUILD_DOXYGEN)
  find_package(Doxygen QUIET)
  if(DOXYGEN_FOUND)
    add_subdirectory (doc)
  else(DOXYGEN_FOUND)
    message(STATUS "Doxygen not found. Documentation will not be built.")
  endif(DOXYGEN_FOUND)
endif (BUILD_DOXYGEN)

message(STATUS "Install ${CMAKE_INSTALL_PREFIX}")

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)


