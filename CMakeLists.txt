cmake_minimum_required(VERSION 3.14)
project(CardCountingSimulator CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories(include)

add_library(bjcore
    src/card.cpp src/shoe.cpp src/hand.cpp src/counter.cpp
    src/edge_table.cpp src/betting.cpp src/game.cpp src/ui.cpp)
if(NOT MSVC)
    target_compile_options(bjcore PRIVATE -Wall -Wextra)
endif()

add_executable(blackjack_sim src/main.cpp)
target_link_libraries(blackjack_sim bjcore)

# ---- Google Test (downloaded automatically the first time you configure) ----
include(FetchContent)
FetchContent_Declare(googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
file(GLOB TEST_SOURCES ${CMAKE_SOURCE_DIR}/tests/*_test.cpp)
add_executable(run_tests ${TEST_SOURCES})
target_compile_definitions(run_tests PRIVATE DATA_DIR="${CMAKE_SOURCE_DIR}/data")
target_link_libraries(run_tests bjcore GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(run_tests)
