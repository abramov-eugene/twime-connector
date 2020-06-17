cmake_minimum_required(VERSION 3.0)

find_package(Boost 1.71.0  COMPONENTS asio)



project(alfalab)

add_executable(alfalab main.cpp)

target_link_libraries(alfalab Boost::asio)

install(TARGETS alfalab RUNTIME DESTINATION bin)
