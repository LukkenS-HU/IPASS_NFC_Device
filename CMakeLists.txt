cmake_minimum_required(VERSION 3.17)

include(pico-sdk/pico_sdk_init.cmake)

project(IPASS_NFC_Device)
set(CMAKE_CXX_STANDARD 20)

pico_sdk_init()

add_subdirectory(lib)
add_subdirectory(tests)

HWLIB_AddExecutable(IPASS_NFC_Device main.cpp DebouncedPin.cpp DebouncedPin.h)
HWLIB_TargetLinkLibraries(IPASS_NFC_Device PN532 PN532-hwlib PNHelpers)