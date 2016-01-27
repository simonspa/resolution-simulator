# - Try to find pxarCore used in native reader library for exporting RAW to pxarCore format needed for reconstruction
# Once done this will define
#  GBL_FOUND - System has GBL
#  GBL_INCLUDE_DIR - The GBL main include directories
#  GBL_LIBRARY - The libraries needed to use GBL

MESSAGE(STATUS "Looking for GBL...")

find_path(GBL_INCLUDE_DIR GblPoint.h
  HINTS "${GBLPATH}/include" "$ENV{GBLPATH}/include" "${GBLPATH}/cpp/include" "$ENV{GBLPATH}/cpp/include")

find_library(GBL_LIBRARY NAMES GBL
  HINTS "${GBLPATH}/lib" "${GBLPATH}/cpp/lib" "$ENV{GBLPATH}/lib" "$ENV{GBLPATH}/cpp/lib")

MESSAGE(STATUS "include: ${GBL_INCLUDE_DIR} ${GBL_LIBRARY}")

#include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set GBL_FOUND to TRUE
# if all listed variables are TRUE
#find_package_handle_standard_args(pxarCore
#  REQUIRED_VARS GBL_LIBRARY GBL_API_INCLUDE_DIR GBL_UTILS_INCLUDE_DIR PXAR_UTIL_INCLUDE_DIR)

IF(GBL_LIBRARY AND GBL_INCLUDE_DIR)
   SET(GBL_FOUND TRUE)
   MESSAGE(STATUS "Found GBL library and headers.")
ELSE(GBL_LIBRARY AND GBL_INCLUDE_DIR)
  SET(GBL_FOUND FALSE)
ENDIF(GBL_LIBRARY AND GBL_INCLUDE_DIR)

mark_as_advanced(GBL_LIBRARY GBL_INCLUDE_DIR)