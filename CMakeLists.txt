#
cmake_minimum_required(VERSION 3.6)

# set the project name and version
project(IOHalgorithm VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# specify the CXX FLAGS
set(CMAKE_CXX_FLAGS "-g -std=c++11 -Wall -Wno-unused-variable -Wno-unused-function -I/usr/local/include/ -L/usr/local/lib/ -lIOH")

# set source file directorys and prepare for adding libraries.
SET(SRC "src/")
FILE (GLOB SRC_CPP "${SRC}/*.cpp")
FILE (GLOB SRC_INCLUDE "${SRC}/*.h")

SET(IOHALGORITHM_CPP
  "${SRC_CPP}"
)

SET(IOHEXPERIMENTER_INCLUDE
  "${SRC_INCLUDE}"
)

# add the IOH library
add_library(configGA ${SRC_CPP})

# install. set name of the installed library as 'IOH'.
install (TARGETS configGA DESTINATION lib)
install (FILES ${IOHEXPERIMENTER_INCLUDE} DESTINATION include)
