cmake_minimum_required(VERSION 3.28)

project(divacon_gold_src)

add_subdirectory(buttons)
add_subdirectory(common)
add_subdirectory(log)
add_subdirectory(menu)
add_subdirectory(peripheral)
add_subdirectory(slider)
add_subdirectory(system)
add_subdirectory(usb)

project(divacon_gold_main VERSION 0.0.1 LANGUAGES ASM C CXX)

set(SRCS
  main.c
)

add_executable(${PROJECT_NAME} ${SRCS})

if(NOT UNIT_TESTING)
  # USB stdio
  # pico_enable_stdio_usb(${PROJECT_NAME} ON)
  # pico_enable_stdio_uart(${PROJECT_NAME} OFF)

  # UART stdio
  pico_enable_stdio_usb(${PROJECT_NAME} OFF)
  pico_enable_stdio_uart(${PROJECT_NAME} ON)

  # create map/bin/hex/uf2 file in addition to ELF.
  pico_add_extra_outputs(${PROJECT_NAME})

  # Fix for Errata RP2040-E5 (the fix requires use of GPIO 15)
  target_compile_definitions(${PROJECT_NAME} PUBLIC
    PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1
  )

  if(PICO_ON_DEVICE)
    message("TinyUSB submodule not initialised in Pico SDK.")
  endif()
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(${PROJECT_NAME} PUBLIC
  divacon-buttons
  divacon-common
  divacon-menu
  divacon-slider
  divacon-system
  divacon-usb

  hardware_structs
  hardware_i2c
  pico_stdlib
)

# flash_divacon_gold_main CMake target
# Target to flash the firmware using Pico SDK OpenOCD Version
set(OPENOCD_PATH "${PICO_SDK_PATH}/../../openocd/0.12.0+dev" CACHE PATH "Path to the PicoSDK OpenOCD executable")

add_custom_target(flash_${PROJECT_NAME}
  DEPENDS
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.elf
  COMMAND
  ${OPENOCD_PATH}/openocd -s ${OPENOCD_PATH}/scripts -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000; program ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.elf verify reset exit"
  VERBATIM
)

add_dependencies(flash_${PROJECT_NAME} ${PROJECT_NAME})
