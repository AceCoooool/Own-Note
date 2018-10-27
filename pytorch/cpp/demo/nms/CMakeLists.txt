cmake_minimum_required(VERSION 3.8)
project(nms)

set(CMAKE_CXX_STANDARD 14)

find_package(Torch REQUIRED)
find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})

set(SOURCE_FILES main.cpp nms_cpu.cpp nms_cpu.h)
add_executable(nms ${SOURCE_FILES})
target_link_libraries(nms "${TORCH_LIBRARIES}")
target_link_libraries(nms "${PYTHON_LIBRARIES}")
