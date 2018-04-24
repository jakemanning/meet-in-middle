cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(middle)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror -pedantic -g -O2 -pthread -march=native")
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(NTL REQUIRED)
find_package(GMP REQUIRED)

add_executable(middle random_subsets.cpp)

target_link_libraries(middle ${NTL_LIBRARY} ${GMP_LIBRARIES} m)
target_include_directories(middle PUBLIC /usr/local/include /usr/local/Cellar/nlohmann_json/3.1.2/include)
