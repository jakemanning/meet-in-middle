cmake_minimum_required(VERSION 3.10.3 FATAL_ERROR)
project(middle)
project(random)
project(testing)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(NTL REQUIRED)
find_package(GMP REQUIRED)
find_package(Boost REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2 -Wall -pedantic -g -pthread -march=native")

add_executable(random random_subsets.cpp util.h)
add_executable(middle meet_in_middle.cpp util.h)
add_executable(testing test.cpp)

target_link_libraries(middle ${NTL_LIBRARY} ${GMP_LIBRARIES} m)
target_link_libraries(random ${NTL_LIBRARY} ${GMP_LIBRARIES} m)
target_link_libraries(testing ${NTL_LIBRARY} ${GMP_LIBRARIES} m)
target_include_directories(middle PUBLIC /usr/local/include /usr/local/Cellar/nlohmann_json/3.1.2/include /home/linuxbrew/.linuxbrew/Cellar/nlohmann_json/3.1.2/include)
target_include_directories(random PUBLIC /usr/local/include /usr/local/Cellar/nlohmann_json/3.1.2/include /home/linuxbrew/.linuxbrew/Cellar/nlohmann_json/3.1.2/include)
target_include_directories(testing PUBLIC /usr/local/include /usr/local/Cellar/nlohmann_json/3.1.2/include /home/linuxbrew/.linuxbrew/Cellar/nlohmann_json/3.1.2/include)
