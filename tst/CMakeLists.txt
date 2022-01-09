cmake_minimum_required(VERSION 3.14)
project(my_project)

# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 14)

include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

FetchContent_MakeAvailable(googletest)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -O2")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

enable_testing()

add_executable(
  test_fixed
  test_fixed.cc
)

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
)

target_include_directories(
  test_fixed PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
)

target_include_directories(
  test_particle PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/component>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/physics>
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
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/simulation>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/physics>
)
target_link_directories(

  test_sim PUBLIC
)

target_link_libraries(
  test_fixed
  gtest_main
)

target_link_libraries(
  test_particle
  ${CMAKE_CURRENT_SOURCE_DIR}/../obj/component/vector.o
  ${CMAKE_CURRENT_SOURCE_DIR}/../obj/component/particle.o
  ${CMAKE_CURRENT_SOURCE_DIR}/../obj/physics/physics.o
  gtest_main
)

target_link_libraries(
  test_vector
  ${CMAKE_CURRENT_SOURCE_DIR}/../obj/component/vector.o
  gtest_main
)

target_link_libraries(
  test_sim
  ${CMAKE_CURRENT_SOURCE_DIR}/../obj/simulation/simulation.o
  ${CMAKE_CURRENT_SOURCE_DIR}/../obj/component/vector.o
  ${CMAKE_CURRENT_SOURCE_DIR}/../obj/component/particle.o
  ${CMAKE_CURRENT_SOURCE_DIR}/../obj/physics/physics.o
  gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_vector test_sim test_particle test_fixed)

