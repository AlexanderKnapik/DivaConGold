cmake_minimum_required(VERSION 3.13)

project(pio_ws2812 VERSION 0.0.1 LANGUAGES ASM C)

set(SRCS
  src/ws2812.c
)

set(HDRS
  include/pio_ws2812/ws2812.h
)

add_library(pio_ws2812 ${SRCS})

pico_generate_pio_header(${PROJECT_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ws2812.pio

  OUTPUT_DIR
  ${CMAKE_CURRENT_BINARY_DIR}/include
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

  PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
)

target_link_libraries(${PROJECT_NAME} PUBLIC
  pico_stdlib
  hardware_pio
)
