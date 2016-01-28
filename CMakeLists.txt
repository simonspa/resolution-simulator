########################################################
# CMake file for COCPITT telescope simulations
CMAKE_MINIMUM_REQUIRED(VERSION 2.6 FATAL_ERROR)
IF(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)
########################################################

# Project name
PROJECT( ccpsim )

# Setup the build environment
LIST ( APPEND CMAKE_CXX_FLAGS "-fPIC -O2 -std=c++11" )
LIST ( APPEND CMAKE_LD_FLAGS "-fPIC -O2" )

# Additional packages to be searched for by cmake
LIST( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )

SET(INSTALL_PREFIX "${PROJECT_SOURCE_DIR}" CACHE PATH "Prefix prepended to install directories")
SET(CMAKE_INSTALL_PREFIX "${INSTALL_PREFIX}" CACHE INTERNAL "Prefix prepended to install directories" FORCE)

SET(SOURCE_FILES
  "telescope/propagate.cc"
  "telescope/assembly.cc"
  )

# Depends on GBL for tracking and ROOT for plotting:
FIND_PACKAGE(ROOT REQUIRED)
FIND_PACKAGE(GBL REQUIRED)

INCLUDE_DIRECTORIES(SYSTEM telescope utils ${ROOT_INCLUDE_DIR} ${GBL_INCLUDE_DIR})

ADD_EXECUTABLE(tpad devices/tscope_pads.cc ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(tpad ${ROOT_LIBRARIES} ${GBL_LIBRARY})

ADD_EXECUTABLE(tpix devices/tscope_diamondpixel.cc ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(tpix ${ROOT_LIBRARIES} ${GBL_LIBRARY})
