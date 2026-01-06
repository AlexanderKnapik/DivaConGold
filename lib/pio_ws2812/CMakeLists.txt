cmake_minimum_required(VERSION 3.13)

project(pio_ws2812 VERSION 0.0.1 LANGUAGES ASM C)

set(SRCS
  ws2812.c
)

set(HDRS
  ws2812.h
)

add_library(pio_ws2812 ${SRCS})

if(NOT UNIT_TESTING)
  pico_generate_pio_header(${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/ws2812.pio

    OUTPUT_DIR
    ${CMAKE_CURRENT_BINARY_DIR}
  )
endif()

target_include_directories(${PROJECT_NAME}
  PUBLIC
  $<BUILD_INTERFACE:${divacon_gold_lib_SOURCE_DIR}>

  PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

target_link_libraries(${PROJECT_NAME} PUBLIC
  pico_stdlib
  hardware_pio
)
