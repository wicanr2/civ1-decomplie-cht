cmake_minimum_required(VERSION 3.16)
project(civ1_decomplie_cht C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# ---- 相依性 ----------------------------------------------------------------
find_package(SDL2 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(FREETYPE REQUIRED freetype2)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter)
endif()

# ---- 預設字型路徑（可由 CIV_DEFAULT_FONT env override） ------------------
set(CIV_DEFAULT_FONT_PATH "/usr/share/fonts/truetype/arphic/uming.ttc"
    CACHE STRING "預設 CJK 字型檔（編譯時值；runtime 可由 CIV_FONT env override）")
add_compile_definitions(CIV_DEFAULT_FONT_PATH="${CIV_DEFAULT_FONT_PATH}")

# ---- core library ----------------------------------------------------------
set(CIV_CORE_SOURCES
    src/civ_loop.c
    src/civ_event.c
    src/civ_widgets.c
    src/gfx/surface.c
    src/gfx/palette.c
    src/gfx/primitive.c
    src/gfx/present.c
    src/gfx/cvpc.c
    src/res/rsrcfork.c
    src/res/loader.c
    src/data/strings.c
    src/data/civs.c
    src/data/civ_dict.c
    src/dialog/dialog.c
    src/dialog/wizard.c
    src/text/big5.c
    src/text/utf8.c
    src/text/glyph_cache.c
    src/text/text_out.c
    src/widgets/widget.c
    src/widgets/map.c
    src/widgets/minimap.c
    src/widgets/status.c
)

add_library(civ_core STATIC ${CIV_CORE_SOURCES})
target_include_directories(civ_core PUBLIC
    src
    ${SDL2_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
)
target_link_libraries(civ_core PUBLIC
    ${SDL2_LIBRARIES}
    ${FREETYPE_LIBRARIES}
)
target_compile_options(civ_core PUBLIC ${FREETYPE_CFLAGS_OTHER})

# ---- executable ------------------------------------------------------------
add_executable(civ1 src/main.c)
target_link_libraries(civ1 PRIVATE civ_core)

# ---- tests -----------------------------------------------------------------
enable_testing()
add_subdirectory(tests)
