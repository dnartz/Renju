project(Renju)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES chess.cpp)
add_executable(Renju ${SOURCE_FILES})