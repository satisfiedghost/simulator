cmake_minimum_required(VERSION 3.14)
project(my_project)

# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 11)

include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  test_vector
  test_vector.cc
)

add_executable(
  test_particle
  test_particle.cc
)

add_executable(
  test_sim
  test_sim.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/../obj/simulation/simulation.o
)

target_include_directories(
  test_particle PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/component>
)

target_include_directories(
  test_vector PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/component>
)

target_include_directories(
  test_sim PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/component>
)

target_link_directories(
  test_sim PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../obj/simulation/simulation.o>
)

target_link_libraries(
  test_particle
  gtest_main
)

target_link_libraries(
  test_vector
  gtest_main
)

target_link_libraries(
  test_sim
  gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_vector test_sim)# test_particle)

