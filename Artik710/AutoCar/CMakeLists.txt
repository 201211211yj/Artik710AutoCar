cmake_minimum_required (VERSION 2.8) 

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_MODULE_PATH "/usr/lib/cmake/${CMAKE_MODULE_PATH}") 

find_package(Threads)
find_package(OpenCV)

add_executable (main main.cpp)
  
target_link_libraries (main  ${CMAKE_THREAD_LIBS_INIT})
