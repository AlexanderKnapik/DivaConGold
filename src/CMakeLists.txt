cmake_minimum_required(VERSION 3.28)

project(divacon_gold VERSION 0.39.0 LANGUAGES ASM C CXX)

set(SRCS
  peripherals/ButtonLeds.cpp
  peripherals/Buttons.cpp
  peripherals/Display.cpp
  peripherals/TouchSlider.cpp
  peripherals/TouchSliderLeds.cpp
  # usb/device/hid/common.c
  # usb/device/hid/keyboard_driver.c
  # usb/device/hid/ps3_driver.c
  # usb/device/hid/ps4_auth.c
  # usb/device/hid/ps4_driver.c
  # usb/device/hid/switch_driver.c
  # usb/device/vendor/common.c
  # usb/device/vendor/debug_driver.c
  # usb/device/vendor/pdloader_driver.c
  # usb/device/vendor/xinput_driver.c
  # usb/device/midi_driver.c
  # usb/device_driver.c
  utils/InputReport.cpp
  utils/Menu.cpp
  utils/PS4AuthProvider.cpp
  utils/SettingsStore.cpp
)

add_library(${PROJECT_NAME} ${SRCS})

pico_enable_stdio_usb(${PROJECT_NAME} ON)
pico_enable_stdio_uart(${PROJECT_NAME} OFF)

target_include_directories(${PROJECT_NAME} PRIVATE
  $<BUILD_INTERFACE:${divacon_gold_project_SOURCE_DIR}/src>
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  mpr121
  hardware_i2c
  pico_multicore
  pico_rand
  pico_ssd1306
  pico_stdlib
  pio_ws2812
  pico_mbedtls
  # tinyusb_device
  # tinyusb_board
)
# if(NOT UNIT_TESTING)
#   pico_enable_stdio_usb(${PROJECT_NAME} OFF)
#   pico_enable_stdio_uart(${PROJECT_NAME} ON)
# endif()


project(divacon_gold_main VERSION 0.39.0 LANGUAGES ASM C CXX)

set(SRCS
  main.cpp
)

add_executable(${PROJECT_NAME} ${SRCS})

if(NOT UNIT_TESTING)
  pico_enable_stdio_usb(${PROJECT_NAME} OFF)
  pico_enable_stdio_uart(${PROJECT_NAME} ON)

  # create map/bin/hex/uf2 file in addition to ELF.
  pico_add_extra_outputs(${PROJECT_NAME})
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_include_directories(${PROJECT_NAME} PRIVATE
  $<BUILD_INTERFACE:${divacon_gold_project_SOURCE_DIR}/src>
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  divacon_gold
  hardware_flash
  hardware_i2c
  hardware_pio
  mpr121
  pico_mbedtls
  pico_ssd1306
  pico_stdlib
  # tinyusb_device
)

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
