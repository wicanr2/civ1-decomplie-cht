cmake_minimum_required(VERSION 3.16)
project(civ1_decomplie_cht C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

find_package(SDL2 REQUIRED)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter)
endif()

set(CIV_SOURCES
    src/main.c
    src/civ_loop.c
)

set(CIV_HEADERS
    src/civ_game.h
    src/civ_loop.h
)

add_executable(civ1 ${CIV_SOURCES} ${CIV_HEADERS})
target_include_directories(civ1 PRIVATE src ${SDL2_INCLUDE_DIRS})
target_link_libraries(civ1 PRIVATE ${SDL2_LIBRARIES})

enable_testing()
add_subdirectory(tests)
