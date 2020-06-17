cmake_minimum_required(VERSION 3.0)
set(BOOST_ROOT /usr/include/boost)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(twime CXX)

find_package(Boost 1.71.0 REQUIRED)
find_package(Threads)


include_directories(${Boost_INCLUDE_DIRS})

add_executable(twime main.cpp)

target_link_libraries(twime ${Boost_LIBRARIES} -pthread ${CMAKE_THREAD_LINS_INIT})
set_property(TARGET twime  PROPERTY COMPILE_OPETIONS "-pthread")


install(TARGETS twime RUNTIME DESTINATION bin)
