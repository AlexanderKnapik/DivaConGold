cmake_minimum_required(VERSION 3.13)

project(mpr121 VERSION 0.0.1 LANGUAGES CXX)

set(SRCS
  src/Mpr121.cpp
)

add_library(${PROJECT_NAME} ${SRCS})

target_include_directories(${PROJECT_NAME}
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

  PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/mpr121>
)

target_link_libraries(${PROJECT_NAME} PUBLIC
  pico_stdlib
  hardware_i2c
)
