cmake_minimum_required(VERSION 3.28)

project(divacon-menu VERSION 0.0.1 LANGUAGES C)

set(SRCS
)

set(HDRS
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
  divacon-peripheral

  $<$<NOT:$<BOOL:${UNIT_TESTING}>>:pico_stdlib>
  $<$<NOT:$<BOOL:${UNIT_TESTING}>>:pico_ssd1306>
)

