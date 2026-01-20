cmake_minimum_required(VERSION 3.28)

project(divacon-slider VERSION 0.0.1 LANGUAGES C)

set(SRCS
  # analogue_stick.c
  # animation.c
  # arcade_stick.c
  colour.c
  # mpr121.c
  # slider.c
  # slider_leds.c
  pattern_buffer.c
)

set(HDRS
  # analogue_stick.c
  # animation.h
  # arcade_stick.c
  colour.h
  # mpr121.h
  # slider.h
  # slider_leds.h
  pattern_buffer.c
)

add_library(${PROJECT_NAME} ${SRCS} ${HDRS})

target_include_directories(${PROJECT_NAME}
  PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<$<BOOL:${UNIT_TESTING}>:$<BUILD_INTERFACE:${test_divacon_SOURCE_DIR}/mock>>

  PUBLIC
  $<BUILD_INTERFACE:${divacon_gold_src_SOURCE_DIR}>
)

target_link_libraries(${PROJECT_NAME} PUBLIC
  divacon-common
  divacon-peripheral

  $<$<NOT:$<BOOL:${UNIT_TESTING}>>:pico_stdlib>
  $<$<NOT:$<BOOL:${UNIT_TESTING}>>:pio_ws2812>
)

