cmake_minimum_required(VERSION 3.0)
set(BOOST_ROOT /usr/include/boost)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(twime)

find_package(Boost COMPONENTS asio REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

add_executable(twime main.cpp)

target_link_libraries(twime ${Boost_LIBRARIES})

install(TARGETS twime RUNTIME DESTINATION bin)
