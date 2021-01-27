cmake_minimum_required(VERSION 3.12)

# Pull in PICO SDK (must be before project)
include(pico_sdk_import.cmake)

project(PiPicoUSBHIDKeyboard C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

add_executable(PiPicoUSBHIDKeyboard
        pi_pico_usb_hid_keyboard.c
        usb_descriptors.c
        )


target_include_directories(PiPicoUSBHIDKeyboard PRIVATE ${CMAKE_CURRENT_LIST_DIR})

# link required libraries
target_link_libraries(PiPicoUSBHIDKeyboard PRIVATE pico_stdlib tinyusb_device tinyusb_board)

# create map/bin/hex file etc.
pico_add_extra_outputs(PiPicoUSBHIDKeyboard)

# set the program url
set(URL_BASE "https://github.com/bdtower8/PiPicoUSBHIDKeyboard/tree/HEAD")
file(RELATIVE_PATH URL_REL_PATH "${PROJECT_SOURCE_DIR}" "${CMAKE_CURRENT_LIST_DIR}")
pico_set_program_url(PiPicoUSBHIDKeyboard "${URL_BASE}/${URL_REL_PATH}")
