cmake_minimum_required(VERSION 3.2.0)
project(alignn VERSION 0.1.0)

aux_source_directory(. TEST_LIB_SRCS)
add_library(testLib ${TEST_LIB_SRCS})