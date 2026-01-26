cmake_minimum_required(VERSION 3.28)

project(divacon-slider VERSION 0.0.1 LANGUAGES C)

set(SRCS
  animation.c
  colour.c
  mpr121.c
  pattern_buffer.c
  slider_leds.c
)

set(HDRS
  animation.h
  colour.h
  mpr121.h
  pattern_buffer.c
  slider_leds.h
)

add_library(${PROJECT_NAME} ${SRCS} ${HDRS})

target_include_directories(${PROJECT_NAME}
  PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<$<BOOL:${UNIT_TESTING}>:$<BUILD_INTERFACE:${test_divacon_SOURCE_DIR}/mock>>

  PUBLIC
  $<BUILD_INTERFACE:${divacon_gold_src_SOURCE_DIR}>
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  divacon-common
  divacon-peripheral

  $<$<NOT:$<BOOL:${UNIT_TESTING}>>:hardware_structs>
  $<$<NOT:$<BOOL:${UNIT_TESTING}>>:hardware_gpio>
  $<$<NOT:$<BOOL:${UNIT_TESTING}>>:pico_stdlib>
  $<$<NOT:$<BOOL:${UNIT_TESTING}>>:pio_ws2812>
)

