########################################################
# CMake file for telescope resolution calculator
CMAKE_MINIMUM_REQUIRED(VERSION 3.6.3 FATAL_ERROR)
IF(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
  CMAKE_POLICY(SET CMP0048 NEW) # set project version
ENDIF(COMMAND CMAKE_POLICY)
########################################################

# Project name
PROJECT(telressim VERSION 2.99 LANGUAGES CXX)

###############################
# Setup the build environment #
###############################

# Additional packages to be searched for by cmake
LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Configure the installation prefix to allow both local as system-wide installation
IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    SET(CMAKE_INSTALL_PREFIX
        "${PROJECT_SOURCE_DIR}"
        CACHE PATH "Prefix prepended to install directories" FORCE)
ENDIF()

# Set up the RPATH so executables find the libraries even when installed in non-default location
SET(CMAKE_MACOSX_RPATH TRUE)
SET(CMAKE_SKIP_BUILD_RPATH FALSE)
SET(CMAKE_SKIP_INSTALL_RPATH FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
SET(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_FULL_LIBDIR})

# Add the automatically determined parts of the RPATH which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# The RPATH to be used when installing, but only if it's not a system directory
LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" IsSystemDir)
IF("${IsSystemDir}" STREQUAL "-1")
    SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
ENDIF("${IsSystemDir}" STREQUAL "-1")

IF(APPLE)
    SET(CMAKE_INSTALL_NAME_DIR "@rpath")
    SET(CMAKE_INSTALL_RPATH "@loader_path/../lib") # self relative LIBDIR
ENDIF()

# If available, use CMake's link-what-you-use feature, not on APPLE
IF(NOT APPLE AND NOT CMAKE_LINK_WHAT_YOU_USE)
    SET(CMAKE_LINK_WHAT_YOU_USE
        TRUE
        CACHE STRING "Choose whether to only link libraries which contain symbols actually used by the target." FORCE)
ENDIF()

######################
# Define build flags #
######################

# Set standard build flags
SET(COMPILER_FLAGS
    -pedantic
    -Wall
    -Wextra
    -Wcast-align
    -Wcast-qual
    -Wconversion
    -Wuseless-cast
    -Wctor-dtor-privacy
    -Wzero-as-null-pointer-constant
    -Wdisabled-optimization
    -Wformat=2
    -Winit-self
    -Wlogical-op
    -Wmissing-declarations
    -Wmissing-include-dirs
    -Wnoexcept
    -Wold-style-cast
    -Wredundant-decls
    -Wsign-conversion
    -Wsign-promo
    -Wstrict-null-sentinel
    -Wstrict-overflow=5
    -Wswitch-default
    -Wundef
    -Wshadow
    -Wformat-security
    -Wdeprecated
    -fdiagnostics-color=auto
    -Wheader-hygiene
    -Wno-overloaded-virtual)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)


######################
# Prerequisistes for #
######################

# ROOT is required for vector and persistency etc
FIND_PACKAGE(ROOT REQUIRED COMPONENTS Geom Core GenVector Hist RIO NO_MODULE)
IF(NOT ROOT_FOUND)
    MESSAGE(FATAL_ERROR "Could not find ROOT, make sure to source the ROOT environment\n"
                        "$ source YOUR_ROOT_DIR/bin/thisroot.sh")
ENDIF()

# Downgrade to C++14 if ROOT is not build with C++17 support
IF(ROOT_CXX_FLAGS MATCHES ".*std=c\\+\\+1[7z].*")
    MESSAGE(STATUS "ROOT was built with C++17 support")
ELSEIF(ROOT_CXX_FLAGS MATCHES ".*std=c\\+\\+.*")
    MESSAGE(FATAL_ERROR "ROOT was built with an unsupported C++ version, C++17 is required: ${ROOT_CXX_FLAGS}")
ELSE()
    MESSAGE(FATAL_ERROR "Could not deduce ROOT's C++ version from build flags: ${ROOT_CXX_FLAGS}")
ENDIF()

# Check ROOT version
IF(NOT ${ROOT_VERSION} VERSION_GREATER "6.0")
    MESSAGE(FATAL_ERROR "ROOT versions below 6.0 are not supported")
ENDIF()


FIND_PACKAGE(Eigen3 REQUIRED)
FIND_PACKAGE(GBL REQUIRED)


########################
# Define build targets #
########################

INCLUDE_DIRECTORIES(telescope utils)
INCLUDE_DIRECTORIES(SYSTEM ${ROOT_INCLUDE_DIR} ${GBL_INCLUDE_DIR} ${EIGEN3_INCLUDE_DIR})

# Build the telescope sim library
ADD_LIBRARY(${PROJECT_NAME} SHARED
  telescope/propagate.cc
  telescope/assembly.cc)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} PUBLIC ${GBL_LIBRARY} Eigen3::Eigen)

# Add subfolder with all telescope devices:
ADD_SUBDIRECTORY(devices)
