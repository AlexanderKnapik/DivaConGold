cmake_minimum_required(VERSION 3.28)

# Pull in SDK (must be before project)
if(NOT UNIT_TESTING)
  set(PICO_SDK_PATH "$ENV{HOME}/.pico-sdk/sdk/2.2.0")
  set(PICOTOOL_FETCH_FROM_GIT_PATH "$ENV{HOME}/.pico-sdk/picotool/2.2.0")
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
  include(pico_sdk_import)
endif()

project(divacon_gold_project VERSION 0.39.0 LANGUAGES C CXX ASM)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_COLOR_DIAGNOSTICS ON)

set(VERBOSE ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(BUILD_SHARED_LIBS OFF)
add_compile_options(-Wall -Wextra -Werror)

if(UNIT_TESTING)
  enable_testing()
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmocka")
  find_package(cmocka CONFIG REQUIRED)
  include(AddCMockaTest)
  add_subdirectory(test)
else()
  set(PICO_DEOPTIMIZED_DEBUG ON)

  if((NOT DEFINED ENV{PICO_SDK_PATH}) AND (NOT PICO_SDK_PATH))
    set(PICO_SDK_FETCH_FROM_GIT_TAG "2.2.0")
    set(PICO_SDK_FETCH_FROM_GIT ON)
  endif()

  if((NOT DEFINED ENV{PICO_BOARD}) AND (NOT PICO_BOARD))
    set(PICO_BOARD "rp2040")
  endif()

  set(PICO_USE_FASTEST_SUPPORTED_CLOCK ON)

  if(NOT DEFINED PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS)
    set(PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS 1000)
  endif()

  pico_sdk_init()
endif()

add_subdirectory(src EXCLUDE_FROM_ALL)
add_subdirectory(lib EXCLUDE_FROM_ALL)

