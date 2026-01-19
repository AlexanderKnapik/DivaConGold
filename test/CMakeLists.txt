cmake_minimum_required(VERSION 3.21)

project(test_divacon LANGUAGES C)

add_cmocka_test(test_pattern_buffer
  SOURCES
  test_pattern_buffer.c

  LINK_LIBRARIES
  cmocka::cmocka
  divacon-slider
  divacon-common
)

target_include_directories(test_pattern_buffer PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/mock
)
