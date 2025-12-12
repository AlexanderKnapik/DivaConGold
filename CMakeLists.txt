cmake_minimum_required(VERSION 3.13)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)
include(pico_extras_import_optional.cmake)

project(divacon_gold_project VERSION 0.39.0 LANGUAGES C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(BUILD_SHARED_LIBS OFF)
set(PICO_DEOPTIMIZED_DEBUG ON)

add_compile_options(-Wall -Wextra -Werror)

if((NOT DEFINED ENV{PICO_SDK_PATH}) AND (NOT PICO_SDK_PATH))
    set(PICO_SDK_FETCH_FROM_GIT_TAG "2.2.0")
    set(PICO_SDK_FETCH_FROM_GIT ON)
endif()

if((NOT DEFINED ENV{PICO_BOARD}) AND (NOT PICO_BOARD))
    set(PICO_BOARD "rp2040")
endif()

set(PICO_USE_FASTEST_SUPPORTED_CLOCK ON)
pico_sdk_init()

add_subdirectory(src)
add_subdirectory(lib)
