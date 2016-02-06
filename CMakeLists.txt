########################################################
# CMake file for COCPITT telescope simulations
CMAKE_MINIMUM_REQUIRED(VERSION 2.6 FATAL_ERROR)
IF(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)
########################################################

# Project name
PROJECT( telressim )

# Setup the build environment
LIST ( APPEND CMAKE_CXX_FLAGS "-fPIC -O2 -std=c++11" )
LIST ( APPEND CMAKE_LD_FLAGS "-fPIC -O2" )

# Additional packages to be searched for by cmake
LIST( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )

SET(INSTALL_PREFIX "${PROJECT_SOURCE_DIR}" CACHE PATH "Prefix prepended to install directories")
SET(CMAKE_INSTALL_PREFIX "${INSTALL_PREFIX}" CACHE INTERNAL "Prefix prepended to install directories" FORCE)

SET(LIB_SOURCE_FILES
  "telescope/propagate.cc"
  "telescope/assembly.cc"
  )

# Depends on GBL for tracking and ROOT for plotting:
FIND_PACKAGE(ROOT REQUIRED)
FIND_PACKAGE(GBL REQUIRED)

INCLUDE_DIRECTORIES(SYSTEM telescope utils ${ROOT_INCLUDE_DIR} ${GBL_INCLUDE_DIR})

# Build the telescope sim library
ADD_LIBRARY(${PROJECT_NAME} SHARED ${LIB_SOURCE_FILES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${GBL_LIBRARY})

# Add subfolder with all telescope devices:
ADD_SUBDIRECTORY(devices)
