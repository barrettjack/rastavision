cmake_minimum_required(VERSION 3.20)

set(CMAKE_C_COMPILER clang-20)
set(CMAKE_CXX_COMPILER clang++-20)

project(renderer)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Default to Debug so stepping through load_obj works out of the box
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

add_executable(renderer
    src/main.cpp
    src/obj_loader.cpp
)

target_include_directories(renderer PRIVATE
    include
    glm-master
)
