cmake_minimum_required(VERSION 3.2.0)
project(alignn VERSION 0.1.0)

aux_source_directory(. IO_LIB_SRCS)
add_library(ioLib ${IO_LIB_SRCS})