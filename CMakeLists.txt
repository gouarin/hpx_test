cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

project(test_hpx CXX)
find_package(HPX)

# add_subdirectory(1d_stencil)
add_subdirectory(2d_stencil)