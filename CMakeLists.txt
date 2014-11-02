cmake_minimum_required(VERSION 2.8.4)
project(ThinkTank)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp StateParser/gamestate.cpp StateParser/mapstate.cpp StateParser/terrain.cpp StateParser/playerstate.cpp StateParser/projectilestate.cpp StateParser/tankstate.cpp StateParser/stateparser.cpp)
add_executable(ThinkTank ${SOURCE_FILES})