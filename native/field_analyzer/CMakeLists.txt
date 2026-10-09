add_executable(WiliPirate main.c)
target_link_libraries(WiliPirate PRIVATE field_ui freewili2_bsp)
target_compile_definitions(WiliPirate PRIVATE FIELD_STANDARD_STARTUP_APPROVED=1)
set_source_files_properties(main.c PROPERTIES COMPILE_OPTIONS "-Wall;-Wextra;-Werror")
pico_enable_stdio_usb(WiliPirate 0)
pico_enable_stdio_uart(WiliPirate 0)
fw2_display_app(WiliPirate
    NAME WiliPirate VERSION 003
    DESCRIPTION "Field Analyzer; simulated CAN and inert Glitching pane; live instruments disabled"
    REPOSITORY "https://github.com/gingerSou1/WiliPirate"
    POWER_ZONES DISPLAY)
# The official helper generates/validates beside the target in this subdirectory.
# Expose one canonical local candidate without touching the historical release.
add_custom_command(TARGET WiliPirate POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_BINARY_DIR}/WiliPirate.uf2"
            "${CMAKE_BINARY_DIR}/WiliPirate.uf2"
    VERBATIM)
