cmake_minimum_required(VERSION 2.8.4)
project(ThinkTank)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

FIND_LIBRARY(ZMQ_LIB zmq)

include_directories("/usr/local/include")

set(SOURCE_FILES main.cpp util.cpp control.cpp Strategies/random_firing_strategy.cpp Strategies/predictive_firing_strategy.cpp Strategies/direct_firing_strategy.cpp Strategies/differential_movement_strategy.cpp Strategies/firing_strategy.cpp Strategies/movement_strategy.cpp commandchannel.cpp StateParser/gamestate.cpp StateParser/mapstate.cpp StateParser/terrain.cpp StateParser/playerstate.cpp StateParser/projectilestate.cpp StateParser/tankstate.cpp StateParser/stateparser.cpp StateParser/positionstate.cpp)
set(TEST_SOURCE_FILES tests.cpp util.cpp control.cpp Strategies/random_firing_strategy.cpp Strategies/predictive_firing_strategy.cpp Strategies/direct_firing_strategy.cpp Strategies/differential_movement_strategy.cpp Strategies/firing_strategy.cpp Strategies/movement_strategy.cpp commandchannel.cpp StateParser/gamestate.cpp StateParser/mapstate.cpp StateParser/terrain.cpp StateParser/playerstate.cpp StateParser/projectilestate.cpp StateParser/tankstate.cpp StateParser/stateparser.cpp StateParser/positionstate.cpp)

add_executable(ThinkTank ${SOURCE_FILES})
add_executable(Test ${TEST_SOURCE_FILES})
TARGET_LINK_LIBRARIES(Test ${ZMQ_LIB})
TARGET_LINK_LIBRARIES(ThinkTank ${ZMQ_LIB})
