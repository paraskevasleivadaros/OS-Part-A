cmake_minimum_required (VERSION 3.13.4)
project (POSIX-Threads)
add_executable(POSIX-Threads p3150173-p3150090-res1.c)
add_compile_options(-pthread)