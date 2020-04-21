cmake_minimum_required(VERSION 3.2)
project(SPPI)

set(CMAKE_CXX_STANDARD 14)

add_library(SPPI SPPI.cpp SPPI.hpp)
add_executable(SPPI_Test test.cpp)
target_link_libraries(SPPI_Test SPPI)
