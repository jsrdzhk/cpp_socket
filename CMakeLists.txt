cmake_minimum_required(VERSION 3.20)
project(cpp_socket)

# set compile parameter
if(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra -fpermissive -rdynamic -std=c++11 -pthread -Wno-unused-parameter -Wno-unused-variable -Wno-missing-field-initializers"
    )
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra -Wno-missing-field-initializers"
    )
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=1 -D_UNICODE -DUNICODE)
endif()

file(GLOB_RECURSE cpp_socket_src ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_library(cppsocket SHARED ${cpp_socket_src})
target_include_directories(cppsocket
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
