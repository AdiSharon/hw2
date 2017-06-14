cmake_minimum_required(VERSION 3.7)
project(list)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES list_mtm1.h list_mtm1.c list_example_test.c test_utilities.h)
add_library(list ${SOURCE_FILES})