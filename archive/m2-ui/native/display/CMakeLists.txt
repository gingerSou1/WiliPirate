add_executable(WiliPirate main.c ui.c)
target_include_directories(WiliPirate PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(WiliPirate PRIVATE freewili2_bsp)
target_compile_options(WiliPirate PRIVATE -Wall -Wextra)
# SDK interface libraries add their own sources to the application target.
# Keep our code strict without changing the unmodified dependencies' policy.
set_source_files_properties(main.c ui.c PROPERTIES COMPILE_OPTIONS "-Werror")
pico_enable_stdio_usb(WiliPirate 0)
pico_enable_stdio_uart(WiliPirate 0)
fw2_display_app(WiliPirate
    NAME WiliPirate
    VERSION 001
    DESCRIPTION "Six-interface touch panel; UI-only M2 prototype"
    REPOSITORY "https://github.com/gingerSou1/WiliPirate"
    POWER_ZONES DISPLAY)
