PlayStation bare-metal project. 

The project setup code is located in PSX_Bare_Metal/cmake.
Code relating to the C Standard is located in PSX_Bare_Metal/src/libc.
Code that interacts with the hardware is located in PSX_Bare_Metal/src/hardware.

The MIPS GCC compiler can be downloaded at:
https://static.grumpycoder.net/pixel/mips/

The recommended build system is Ninja, found at:
https://ninja-build.org/

Steps to compile the project:

1. Install both prerequisites, and add them to PATH.
2. Create a build folder in PSX_Bare_Metal and switch to it.
3. run "cmake .. -G Ninja", followed by "ninja".

The resulting main.elf can be converted into a PSX-EXE format when placed in a folder which is converted into a .bin using this tool:
https://github.com/EmrahVeladzic/DIR_2_PSX
