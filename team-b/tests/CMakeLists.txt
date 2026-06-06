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

add_executable(test_widget_dispatch test_widget_dispatch.c)
target_link_libraries(test_widget_dispatch PRIVATE civ_core)
add_test(NAME widget_dispatch COMMAND test_widget_dispatch)
set_tests_properties(widget_dispatch PROPERTIES
    ENVIRONMENT "SDL_VIDEODRIVER=dummy"
    TIMEOUT 10
)

add_executable(test_rsrc_fork test_rsrc_fork.c)
target_link_libraries(test_rsrc_fork PRIVATE civ_core)
add_test(NAME rsrc_fork COMMAND test_rsrc_fork)
set_tests_properties(rsrc_fork PROPERTIES TIMEOUT 10)

add_executable(test_cvpc_decode test_cvpc_decode.c)
target_link_libraries(test_cvpc_decode PRIVATE civ_core)
add_test(NAME cvpc_decode COMMAND test_cvpc_decode)
set_tests_properties(cvpc_decode PROPERTIES TIMEOUT 10)

add_executable(test_strings test_strings.c)
target_link_libraries(test_strings PRIVATE civ_core)
add_test(NAME strings COMMAND test_strings)
set_tests_properties(strings PROPERTIES TIMEOUT 10)

add_executable(test_civ_dict test_civ_dict.c)
target_link_libraries(test_civ_dict PRIVATE civ_core)
add_test(NAME civ_dict COMMAND test_civ_dict)
set_tests_properties(civ_dict PROPERTIES TIMEOUT 10)

add_executable(test_palette_remap test_palette_remap.c)
target_link_libraries(test_palette_remap PRIVATE civ_core)
add_test(NAME palette_remap COMMAND test_palette_remap)
set_tests_properties(palette_remap PROPERTIES TIMEOUT 10)

add_executable(test_wizard test_wizard.c)
target_link_libraries(test_wizard PRIVATE civ_core)
add_test(NAME wizard COMMAND test_wizard)
set_tests_properties(wizard PROPERTIES
    ENVIRONMENT "SDL_VIDEODRIVER=dummy"
    TIMEOUT 10
)

add_executable(test_wizard_snapshot test_wizard_snapshot.c)
target_link_libraries(test_wizard_snapshot PRIVATE civ_core)
add_test(NAME wizard_snapshot COMMAND test_wizard_snapshot)
set_tests_properties(wizard_snapshot PROPERTIES
    ENVIRONMENT "SDL_VIDEODRIVER=dummy"
    TIMEOUT 30
)
