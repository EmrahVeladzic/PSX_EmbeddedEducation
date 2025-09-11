cmake_minimum_required(VERSION 4.0.2)

set(CMAKE_EXECUTABLE_SUFFIX .elf)
set(CMAKE_STATIC_LIBRARY_PREFIX lib)
set(CMAKE_STATIC_LIBRARY_SUFFIX .a)

link_libraries(-lgcc)

add_library(compilation_flags INTERFACE)
link_libraries(compilation_flags)

target_compile_features(
    compilation_flags INTERFACE
    c_std_17
)

target_compile_options(
    compilation_flags INTERFACE

        -g
        -Wall
        -Wa,--strip-local-absolute
        -ffreestanding
        -fno-builtin
        -fno-pic
        -nostdlib
        -fdata-sections
        -ffunction-sections
        -fsigned-char
        -fno-strict-overflow
        -march=r3000
        -mabi=32
        -mfp32
        -mno-mt
        -mno-llsc
        -mno-abicalls
        -mgpopt
        -mno-extern-sdata
        -G8

)

target_link_options(
    compilation_flags INTERFACE

        -static
        -nostdlib
        -Wl,-gc-sections
        -mgpopt
        -G8
        "-T${CMAKE_CURRENT_LIST_DIR}/link.ld"

)