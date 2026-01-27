cmake_minimum_required(VERSION 3.28)

project(divacon-usb VERSION 0.0.1 LANGUAGES C)

set(SRCS
    hid.c
    keyboard.c
    usb.c
    usb_descriptors.c

)

set(HDRS
    hid.h
    keyboard.h
    usb.h
    usb_descriptors.h
)

add_library(${PROJECT_NAME} ${SRCS} ${HDRS})

# Fix for Errata RP2040-E5 (the fix requires use of GPIO 15)
target_compile_definitions(${PROJECT_NAME} PUBLIC
    PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1
)

if(PICO_ON_DEVICE)
    message("TinyUSB submodule not initialised in Pico SDK.")
endif()

target_include_directories(${PROJECT_NAME}
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>

    PUBLIC
    $<BUILD_INTERFACE:${divacon_gold_src_SOURCE_DIR}>
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    divacon-common
    divacon-buttons
    divacon-system

    pico_stdlib
    pico_unique_id
    tinyusb_device
    tinyusb_board
)
