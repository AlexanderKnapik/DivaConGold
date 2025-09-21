cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(BUILD_SHARED_LIBS OFF)
add_compile_options(-Wall -Wextra -Werror)

include(pico_sdk_import.cmake)

project(divacon_gold_project VERSION 0.39.0)

if((NOT DEFINED ENV{PICO_SDK_PATH}) AND (NOT PICO_SDK_PATH))
    set(PICO_SDK_FETCH_FROM_GIT_TAG "2.2.0")
    set(PICO_SDK_FETCH_FROM_GIT ON)
endif()

if((NOT DEFINED ENV{PICO_BOARD}) AND (NOT PICO_BOARD))
    set(PICO_BOARD "pico")
endif()

set(PICO_USE_FASTEST_SUPPORTED_CLOCK ON)
pico_sdk_init()

add_subdirectory(src)
add_subdirectory(lib)
