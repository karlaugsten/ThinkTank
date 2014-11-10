cmake_minimum_required(VERSION 2.8.4)
project(ThinkTank)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -ggdb")

FIND_LIBRARY(ZMQ_LIB zmq)

include_directories("/usr/local/include")

set(SOURCE_FILES main.cpp commandchannel.cpp StateParser/gamestate.cpp StateParser/mapstate.cpp StateParser/terrain.cpp StateParser/playerstate.cpp StateParser/projectilestate.cpp StateParser/tankstate.cpp StateParser/stateparser.cpp StateParser/positionstate.cpp)
add_executable(ThinkTank ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(ThinkTank ${ZMQ_LIB})
