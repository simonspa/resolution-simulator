#!/bin/bash

# Determine which OS you are using
if [ "$(uname)" == "Linux" ]; then
    if [ "$( cat /etc/*-release | grep Scientific )" ]; then
        OS=slc6
    elif [ "$( cat /etc/*-release | grep CentOS )" ]; then
        OS=centos7
    else
        echo "Cannot detect OS, falling back to CERN Centos 7"
        OS=centos7
    fi
else
    echo "Unknown OS"
    exit 1
fi

# Determine is you have CVMFS installed
if [ ! -d "/cvmfs" ]; then
    echo "No CVMFS detected, please install it."
    exit 1
fi

if [ ! -d "/cvmfs/clicdp.cern.ch" ]; then
    echo "No clicdp CVMFS repository detected, please add it."
    exit 1
fi

# Determine which compiler to use
if [ -z ${COMPILER_TYPE} ]; then
    echo "No compiler type set, falling back to GCC."
    COMPILER_TYPE="gcc"
fi
if [ ${COMPILER_TYPE} == "gcc" ]; then
    echo "Compiler type set to GCC."
    COMPILER_VERSION="gcc8"
fi
if [ ${COMPILER_TYPE} == "llvm" ]; then
    echo "Compiler type set to LLVM."
    COMPILER_VERSION="llvm6"
fi

# Choose build type
if [ -z ${BUILD_TYPE} ]; then
    BUILD_TYPE=opt
fi

# General variables
CLICREPO=/cvmfs/clicdp.cern.ch
export BUILD_FLAVOUR=x86_64-${OS}-${COMPILER_VERSION}-${BUILD_TYPE}

#--------------------------------------------------------------------------------
#     Compiler
#--------------------------------------------------------------------------------

if [ ${COMPILER_TYPE} == "gcc" ]; then
    source ${CLICREPO}/compilers/gcc/8.1.0/x86_64-${OS}/setup.sh
fi
if [ ${COMPILER_TYPE} == "llvm" ]; then
    source ${CLICREPO}/compilers/llvm/6.0.0/x86_64-${OS}/setup.sh
fi

#--------------------------------------------------------------------------------
#     CMake
#--------------------------------------------------------------------------------

export CMAKE_HOME=${CLICREPO}/software/CMake/3.11.1/${BUILD_FLAVOUR}
export PATH=${CMAKE_HOME}/bin:$PATH

#--------------------------------------------------------------------------------
#     ROOT
#--------------------------------------------------------------------------------

export ROOTSYS=${CLICREPO}/software/ROOT/6.12.06/${BUILD_FLAVOUR}
export PYTHONPATH="$ROOTSYS/lib:$PYTHONPATH"
export PATH="$ROOTSYS/bin:$PATH"
export LD_LIBRARY_PATH="$ROOTSYS/lib:$LD_LIBRARY_PATH"
export CMAKE_PREFIX_PATH="$ROOTSYS:$CMAKE_PREFIX_PATH"

#--------------------------------------------------------------------------------
#     Eigen
#--------------------------------------------------------------------------------

export Eigen_HOME=${CLICREPO}/software/Eigen/3.3.4/${BUILD_FLAVOUR}/
export Eigen3_DIR=${Eigen_HOME}/share/eigen3/cmake/
export CMAKE_PREFIX_PATH="$Eigen3_DIR:$CMAKE_PREFIX_PATH"

#--------------------------------------------------------------------------------
#     GeneralBrokenLines
#--------------------------------------------------------------------------------

export GBLPATH=${CLICREPO}/software/GeneralBrokenLines/2.1.3/${BUILD_FLAVOUR}/
export CMAKE_PREFIX_PATH="$GBLPATH:$CMAKE_PREFIX_PATH"
