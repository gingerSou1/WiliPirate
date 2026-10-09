cmake_minimum_required(VERSION 3.20)

# Standalone project: never configure or build the frozen six-tile M2 target.
set(WILIBSP_PATH "${CMAKE_CURRENT_LIST_DIR}/../../wilibsp" CACHE PATH "Unmodified pinned WiliBSP checkout")
set(ONEWILI_PATH "${WILIBSP_PATH}/libs/onewili" CACHE PATH "Unmodified DISPLAY OneWili checkout")
set(PICO_BOARD freewili2 CACHE STRING "Board type")
list(APPEND PICO_BOARD_HEADER_DIRS "${WILIBSP_PATH}/bsp/boards")
set(PICO_PIO_USB_PATH "${WILIBSP_PATH}/bsp/third_party/Pico-PIO-USB")
set(PICO_SDK_FETCH_FROM_GIT OFF CACHE BOOL "Require explicit SDK sources")
include("${WILIBSP_PATH}/pico_sdk_import.cmake")
project(WiliPirateI2CDiagnostic C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
find_package(Python3 COMPONENTS Interpreter REQUIRED)
set(FW2_AGENTIO OFF CACHE BOOL "No remote-control harness" FORCE)
pico_sdk_init()
if(NOT PICO_SDK_VERSION_STRING STREQUAL "2.3.0")
    message(FATAL_ERROR "Diagnostic requires Pico SDK 2.3.0")
endif()
add_subdirectory("${WILIBSP_PATH}/bsp" "${CMAKE_BINARY_DIR}/bsp")
add_subdirectory("${ONEWILI_PATH}/wilibsp" "${CMAKE_BINARY_DIR}/onewili")
add_executable(WiliPirateI2CDiag main.c capture.c)
target_link_libraries(WiliPirateI2CDiag PRIVATE freewili2_bsp onewili_fwgui)
target_compile_options(WiliPirateI2CDiag PRIVATE -Wall -Wextra)
set_source_files_properties(main.c capture.c PROPERTIES COMPILE_OPTIONS "-Werror")
pico_enable_stdio_usb(WiliPirateI2CDiag 0)
pico_enable_stdio_uart(WiliPirateI2CDiag 0)
fw2_display_app(WiliPirateI2CDiag
    NAME WiliPirateI2CDiag
    VERSION 001
    DESCRIPTION "One manual stock I2C Poll; raw diagnostic only"
    REPOSITORY "https://github.com/gingerSou1/WiliPirate"
    POWER_ZONES DISPLAY)
