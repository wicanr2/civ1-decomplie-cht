add_executable(test_window_lifecycle
    test_window_lifecycle.c
    ../src/civ_loop.c
)
target_include_directories(test_window_lifecycle PRIVATE
    ../src
    ${SDL2_INCLUDE_DIRS}
)
target_link_libraries(test_window_lifecycle PRIVATE ${SDL2_LIBRARIES})

add_test(NAME window_lifecycle COMMAND test_window_lifecycle)
set_tests_properties(window_lifecycle PROPERTIES
    ENVIRONMENT "SDL_VIDEODRIVER=dummy;SDL_AUDIODRIVER=dummy"
    TIMEOUT 10
)
