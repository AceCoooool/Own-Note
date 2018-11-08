cmake_minimum_required(VERSION 3.12)
project(MiniSTL)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE main.cpp
        iterator/type_traits.h iterator/stl_iterator.h
        allocator/stl_uninitialized.h allocator/stl_alloc_first.h allocator/stl_alloc_second.h
        allocator/stl_alloc_second.imp.h allocator/stl_alloc.h allocator/stl_construct.h
        containers/vector/stl_vector.h containers/vector/stl_vector.imp.h
        containers/list/stl_list_iterator.h containers/list/stl_list.h containers/list/stl_list.imp.h
        containers/deque/stl_deque_iterator.h containers/deque/stl_deque.h containers/deque/stl_deque.imp.h)


add_executable(MiniSTL ${SOURCE})