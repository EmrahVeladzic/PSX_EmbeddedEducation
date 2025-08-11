cmake_minimum_required(VERSION 4.0.2)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR mipsel)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_program(
    gccPath
        mipsel-none-elf-gcc
        mipsel-unknown-elf-gcc
        mipsel-linux-gnu-gcc
    NO_CACHE
)


if("${gccPath}" STREQUAL "gccPath-NOTFOUND")
  message(FATAL_ERROR "GCC Toolchain not found in PATH.")
endif()

string(REGEX MATCH "^(.+-)gcc(.*)$" dummy "${gccPath}")

set(CMAKE_ASM_COMPILER "${CMAKE_MATCH_1}gcc${CMAKE_MATCH_2}")
set(CMAKE_C_COMPILER   "${CMAKE_MATCH_1}gcc${CMAKE_MATCH_2}")
set(CMAKE_CXX_COMPILER "${CMAKE_MATCH_1}g++${CMAKE_MATCH_2}")
set(CMAKE_AR           "${CMAKE_MATCH_1}ar${CMAKE_MATCH_2}")
set(CMAKE_LINKER       "${CMAKE_MATCH_1}ld${CMAKE_MATCH_2}")
set(CMAKE_RANLIB       "${CMAKE_MATCH_1}ranlib${CMAKE_MATCH_2}")
set(CMAKE_OBJCOPY      "${CMAKE_MATCH_1}objcopy${CMAKE_MATCH_2}")
set(CMAKE_OBJDUMP      "${CMAKE_MATCH_1}objdump${CMAKE_MATCH_2}")
set(CMAKE_NM           "${CMAKE_MATCH_1}nm${CMAKE_MATCH_2}")
set(CMAKE_SIZE         "${CMAKE_MATCH_1}size${CMAKE_MATCH_2}")
set(CMAKE_STRIP        "${CMAKE_MATCH_1}strip${CMAKE_MATCH_2}")
set(CMAKE_READELF      "${CMAKE_MATCH_1}readelf${CMAKE_MATCH_2}")