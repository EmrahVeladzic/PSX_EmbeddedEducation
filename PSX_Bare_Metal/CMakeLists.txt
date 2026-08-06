cmake_minimum_required(VERSION 4.0.2)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/cmake/toolchain.cmake")

project(
    EmbeddedPSX
    LANGUAGES C ASM
    DESCRIPTION "Test project for the PlayStation 1."
)

include(cmake/setup.cmake)

file(GLOB commonSrc
    src/libc/*.c
    src/libc/*.s
    src/hardware/*.c
    src/hardware/*.s
    src/game/*.s
    src/game/*.c
)

add_library(common OBJECT ${commonSrc})

target_include_directories(
    common PUBLIC
    src
    src/libc
    src/hardware
    src/game
)


add_executable(main src/main.c)

target_link_libraries(main PRIVATE common)