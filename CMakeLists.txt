cmake_minimum_required(VERSION 3.11)
project(PhySim)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

link_libraries(${CMAKE_DL_LIBS})

add_subdirectory(extern)
add_subdirectory(commons)
add_subdirectory(proj1)
add_subdirectory(proj2)
add_subdirectory(proj3)