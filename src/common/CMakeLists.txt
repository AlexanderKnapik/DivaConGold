cmake_minimum_required(VERSION 3.28)

project(divacon-common VERSION 0.0.1 LANGUAGES C)

set(SRCS
  slider_leds_common.c
  util.c
  version.c
)

set(HDRS
  bsp.h
  colour_common.h
  error.h
  slider_leds_common.h
  util.h
  version.h
)

add_library(${PROJECT_NAME} ${SRCS} ${HDRS})

target_include_directories(${PROJECT_NAME}
  PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>

  PUBLIC
  $<BUILD_INTERFACE:${divacon_gold_src_SOURCE_DIR}>
)

set_target_properties(${PROJECT_NAME} PROPERTIES
  LINKER_LANGUAGE C
)
