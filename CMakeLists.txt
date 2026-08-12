cmake_minimum_required(VERSION 3.16)
project(atmospheric_reentry_simulator VERSION 1.0.0 LANGUAGES CXX)

option(REENTRY_BUILD_TESTS "Build the unit test executable" ON)

add_library(reentry
    src/atmosphere.cpp
    src/aerodynamics.cpp
    src/simulation.cpp
)
target_include_directories(reentry PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(reentry PUBLIC cxx_std_17)
target_compile_options(reentry PRIVATE $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic>)

add_executable(reentry-sim app/main.cpp)
target_link_libraries(reentry-sim PRIVATE reentry)

if(REENTRY_BUILD_TESTS)
    enable_testing()
    add_executable(reentry-tests tests/reentry_tests.cpp)
    target_link_libraries(reentry-tests PRIVATE reentry)
    add_test(NAME reentry-tests COMMAND reentry-tests)
endif()
