cmake_minimum_required(VERSION 3.0)
set(Boost_DEBUG ON)
set(PROJECT twime)
set(BOOST_COMPONENTS thread system)
set(BOOST_USE_STATIC_LIBS ON)
set(BOOST_USE_MULTITHREADED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


project(${PROJECT})

find_package(Boost COMPONENTS ${BOOST_COMPONENTS} REQUIRED)
find_package(Threads)
if(Boost_FOUND)
   include_directories(${Boost_INCLUDE_DIRS})
   add_executable(${PROJECT} src/main.cpp)
   target_link_libraries(${PROJECT} ${Boost_LIBRARIES} -lpthread ${CMAKE_THREAD_LINS_INIT})
   set_property(TARGET ${PROJECT}  PROPERTY COMPILE_OPETIONS "-pthread")
endif()

#install(TARGETS twime RUNTIME DESTINATION bin)
