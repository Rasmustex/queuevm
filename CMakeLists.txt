cmake_minimum_required(VERSION 3.20)

SET(CMAKE_C_COMPILER       "/usr/bin/clang")
SET(CMAKE_C_FLAGS          "-Wall")
SET(CMAKE_C_FLAGS_DEBUG    "-g")
SET(CMAKE_C_FLAGS_RELEASE  "-O2 -DNDEBUG")

SET(CMAKE_AR      "/usr/bin/llvm-ar")
SET(CMAKE_LINKER  "/usr/bin/llvm-ld")
SET(CMAKE_NM      "/usr/bin/llvm-nm")
SET(CMAKE_OBJDUMP "/usr/bin/llvm-objdump")
SET(CMAKE_RANLIB  "/usr/bin/llvm-ranlib")

project(Qvm VERSION 0.1)

add_subdirectory(lib)
add_subdirectory(src)
