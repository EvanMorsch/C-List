cmake_minimum_required(VERSION 3.25.1)
project(list)

option(TEST "Build all tests." OFF)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wpedantic -Wextra")

include_directories(".")

add_library(${PROJECT_NAME} list.c)

if (TEST)
    add_subdirectory(deps/googletest)

    enable_testing()

    list(APPEND CMAKE_CTEST_ARGUMENTS "--verbose")

    include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

    add_executable(tests test/test_list.cpp)

    target_link_libraries(tests gtest gtest_main)

    target_link_libraries(tests ${PROJECT_NAME})

    add_test(NAME list-tests COMMAND tests)
endif()