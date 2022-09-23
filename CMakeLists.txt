cmake_minimum_required(VERSION 3.2.0)
project(alignn VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#include(CTest)
#enable_testing()
#set(CPACK_PROJECT_NAME ${PROJECT_NAME})
#set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
#include(CPack)

add_subdirectory(src/io)
add_subdirectory(src/interface)
add_subdirectory(src/test)
#include_directories(src/io)
#include_directories(src/interfaces)
find_package(OpenMP REQUIRED)
find_library(
    fCWT
    NAMES fCWT
    REQUIRED
)
add_executable(alignn src/main.cpp)
add_executable(test src/function_test.cpp)
target_link_libraries(alignn ioLib interfaceLib fCWT)
target_link_libraries(test testLib ioLib interfaceLib fCWT)
