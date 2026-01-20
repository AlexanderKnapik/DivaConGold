cmake_minimum_required(VERSION 3.28)

project(divacon-peripheral VERSION 0.0.1 LANGUAGES C)

set(SRCS
    i2c.c
)

set(HDRS
    i2c.h
)

add_library(${PROJECT_NAME} ${SRCS} ${HDRS})

target_include_directories(${PROJECT_NAME}
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>

    PUBLIC
    $<BUILD_INTERFACE:${divacon_gold_src_SOURCE_DIR}>
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    divacon-common

    PRIVATE
    $<$<NOT:$<BOOL:${UNIT_TESTING}>>:hardware_i2c>
)

