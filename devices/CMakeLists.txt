# Get all source files to be compiled as executables:
FILE(GLOB TARGET_FILES
  "*.cc"
)

FOREACH(TFILE ${TARGET_FILES})
  GET_FILENAME_COMPONENT(TNAME ${TFILE} NAME_WE)
  MESSAGE(STATUS "Building device ${TNAME}")
  ADD_EXECUTABLE(${TNAME} ${TFILE})
  TARGET_LINK_LIBRARIES(${TNAME} ${PROJECT_NAME}
    Eigen3::Eigen
    ROOT::Core
    ROOT::GenVector
    ROOT::Geom
    ROOT::RIO
  ROOT::Hist ROOT::Graf3d ${GBL_LIBRARY})
ENDFOREACH()
