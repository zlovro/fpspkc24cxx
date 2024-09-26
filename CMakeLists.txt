cmake_minimum_required(VERSION 3.28)
project(fpspkc24cxx)

set(CMAKE_CXX_STANDARD 26)
#set(CMAKE_CXX_FLAGS -g)
#set(CMAKE_C_FLAGS -g)
#

set(CMAKE_C_FLAGS_DEBUG "-D_DEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-D_DEBUG")

set(CMAKE_CXX_FLAGS "-g -O0")

include_directories(include source/game/backend source source/shaders source/game)
link_directories(lib)

add_executable(fpspkc24cxx
        source/main.cpp
        include/glad/glad.c
        source/game/backend/error_context.hpp
        source/game/backend/variable_bitset.hpp
        source/game/backend/types.hpp
        source/shaders/shader.hpp
        source/game/backend/constants.hpp
        source/game/backend/math_util.hpp
        source/game/backend/kb_chunk_mesh.hpp
        source/game/backend/kb_chunk_archive.hpp
        source/game/backend/binary_reader.hpp
        source/game/backend/kb_util.hpp
        source/game/backend/kb_io.hpp
        source/game/backend/camera.hpp
        source/game/backend/game_object.hpp
        source/game/backend/globals.hpp
        source/game/game.hpp)

target_link_libraries(fpspkc24cxx glfw3dll z)


if (WIN32)
    add_custom_command(
            TARGET fpspkc24cxx POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PROJECT_SOURCE_DIR}/lib/runtime/win64/glfw3.dll" "${CMAKE_BINARY_DIR}"
            COMMAND_EXPAND_LISTS
    )
endif ()