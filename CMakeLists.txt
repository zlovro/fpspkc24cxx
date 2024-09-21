cmake_minimum_required(VERSION 3.28)
project(fpspkc24cxx)

set(CMAKE_CXX_STANDARD 26)
#set(CMAKE_CXX_FLAGS -g)
#set(CMAKE_C_FLAGS -g)
#
#SET(CMAKE_C_FLAGS_DEBUG "-D_DEBUG")
#SET(CMAKE_CXX_FLAGS_DEBUG  "-D_DEBUG")

#set(CMAKE_CXX_FLAGS "-g -O0")

include_directories(include source)
link_directories(lib)

add_executable(fpspkc24cxx
        source/main.cpp
        source/glad.c
        source/shaders/basic_vtx.hlsl.hpp
        source/error_context.hpp
        source/shaders/basic_frag.hlsl.hpp
        source/variable_bitset.hpp
        source/types.hpp
        source/shaders/shader.hpp
        source/constants.hpp
        source/math_util.hpp)

target_link_libraries(fpspkc24cxx glfw3dll)


if (WIN32)
    add_custom_command(
            TARGET fpspkc24cxx POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/lib/runtime/win64/glfw3.dll" "${CMAKE_BINARY_DIR}"
            COMMAND_EXPAND_LISTS
    )
endif ()