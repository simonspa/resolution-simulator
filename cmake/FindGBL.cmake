# - Try to find General Broken Lines library for uncertainty propagation
# Once done this will define
#  GBL_FOUND - System has GBL
#  GBL_INCLUDE_DIR - The GBL main include directories
#  GBL_LIBRARY - The libraries needed to use GBL

MESSAGE(STATUS "Looking for GBL...")

find_path(GBL_INCLUDE_DIR GblPoint.h
  HINTS "${GBLPATH}/include" "$ENV{GBLPATH}/include" "${GBLPATH}/cpp/include" "$ENV{GBLPATH}/cpp/include")

find_library(GBL_LIBRARY NAMES GBL
  HINTS "${GBLPATH}/lib" "${GBLPATH}/cpp/lib" "$ENV{GBLPATH}/lib" "$ENV{GBLPATH}/cpp/lib")

MESSAGE(STATUS "GBL: ${GBL_INCLUDE_DIR} ${GBL_LIBRARY}")

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set GBL_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(GBL 
				  REQUIRED_VARS GBL_LIBRARY GBL_INCLUDE_DIR 
				  FAIL_MESSAGE "Could not find GBL library and headers. Please set the $GBLPATH environment variable.")

mark_as_advanced(GBL_LIBRARY GBL_INCLUDE_DIR)