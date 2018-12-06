cmake_minimum_required(VERSION 3.8)
project(temp)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp 1_naive.h 2_lazy.h 3_exp.h 4_exp_op.h)


include_directories("/home/ace/AceDev/C++/library_read/mshadow")

add_executable(temp ${SOURCE_FILES})