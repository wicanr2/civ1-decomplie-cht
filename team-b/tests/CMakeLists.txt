add_executable(test_window_lifecycle test_window_lifecycle.c)
target_link_libraries(test_window_lifecycle PRIVATE civ_core)
add_test(NAME window_lifecycle COMMAND test_window_lifecycle)
set_tests_properties(window_lifecycle PROPERTIES
    ENVIRONMENT "SDL_VIDEODRIVER=dummy;SDL_AUDIODRIVER=dummy"
    TIMEOUT 10
)

add_executable(test_palette_blit test_palette_blit.c)
target_link_libraries(test_palette_blit PRIVATE civ_core)
add_test(NAME palette_blit COMMAND test_palette_blit)
set_tests_properties(palette_blit PROPERTIES TIMEOUT 10)

add_executable(test_glyph_cache test_glyph_cache.c)
target_link_libraries(test_glyph_cache PRIVATE civ_core)
add_test(NAME glyph_cache COMMAND test_glyph_cache)
set_tests_properties(glyph_cache PROPERTIES TIMEOUT 10)

add_executable(test_demo_snapshot test_demo_snapshot.c)
target_link_libraries(test_demo_snapshot PRIVATE civ_core)
add_test(NAME demo_snapshot COMMAND test_demo_snapshot)
set_tests_properties(demo_snapshot PROPERTIES
    ENVIRONMENT "SDL_VIDEODRIVER=dummy"
    TIMEOUT 30
)
