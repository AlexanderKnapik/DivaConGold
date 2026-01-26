cmake_minimum_required(VERSION 3.28)

project(divacon-buttons VERSION 0.0.1 LANGUAGES C)

set(SRCS
  button_led.c
  buttons.c
)

set(HDRS
  button_led.h
  buttons.h
)

add_library(${PROJECT_NAME} ${SRCS} ${HDRS})

target_include_directories(${PROJECT_NAME}
  PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>

  PUBLIC
  $<BUILD_INTERFACE:${divacon_gold_src_SOURCE_DIR}>
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  divacon-common

  pico_stdlib
)

