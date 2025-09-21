cmake_minimum_required(VERSION 3.13)

project(pico_ssd1306 VERSION 0.0.1 LANGUAGES ASM C)

set(SRCS
  src/ssd1306.c
)

add_library(${PROJECT_NAME} ${SRCS})

target_include_directories(${PROJECT_NAME}
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>

  PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/ssd1306>
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  pico_stdlib
  hardware_i2c
  hardware_dma
)
