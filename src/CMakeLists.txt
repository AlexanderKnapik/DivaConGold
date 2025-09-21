cmake_minimum_required(VERSION 3.13)

project(divacon_gold VERSION 0.39.0 LANGUAGES ASM C CXX)

set(SRCS
  peripherals/ButtonLeds.cpp
  peripherals/Buttons.cpp
  peripherals/Display.cpp
  peripherals/TouchSlider.cpp
  peripherals/TouchSliderLeds.cpp
  usb/device/hid/common.c
  usb/device/hid/keyboard_driver.c
  usb/device/hid/ps3_driver.c
  usb/device/hid/ps4_auth.c
  usb/device/hid/ps4_driver.c
  usb/device/hid/switch_driver.c
  usb/device/vendor/common.c
  usb/device/vendor/debug_driver.c
  usb/device/vendor/pdloader_driver.c
  usb/device/vendor/xinput_driver.c
  usb/device/midi_driver.c
  usb/device_driver.c
  utils/InputState.cpp
  utils/Menu.cpp
  utils/PS4AuthProvider.cpp
  utils/SettingsStore.cpp
)

add_library(${PROJECT_NAME} ${SRCS})

pico_enable_stdio_usb(${PROJECT_NAME} ON)
pico_enable_stdio_uart(${PROJECT_NAME} OFF)

target_include_directories(${PROJECT_NAME} PRIVATE
  $<BUILD_INTERFACE:${divacon_gold_project_SOURCE_DIR}/include>
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  cap1188
  is31se5117a
  mpr121
  hardware_i2c
  pico_multicore
  pico_rand
  pico_ssd1306
  pico_stdlib
  pio_ws2812
  pico_mbedtls
  tinyusb_device
  tinyusb_board
)

project(divacon_gold_main VERSION 0.39.0 LANGUAGES ASM C CXX)

set(SRCS
  main.cpp
)

add_executable(${PROJECT_NAME} ${SRCS})

pico_enable_stdio_usb(${PROJECT_NAME} ON)
pico_enable_stdio_uart(${PROJECT_NAME} OFF)
# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE
  $<BUILD_INTERFACE:${divacon_gold_project_SOURCE_DIR}/include>
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  divacon_gold
  cap1188
  hardware_flash
  hardware_i2c
  hardware_pio
  is31se5117a
  mpr121
  pico_mbedtls
  pico_ssd1306
  pico_stdlib
  tinyusb_device
)
