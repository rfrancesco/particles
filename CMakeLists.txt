cmake_minimum_required(VERSION 3.0.0)
project(Particles VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

find_package(SDL2 REQUIRED)

set(sources
    external_libs/imgui/imconfig.h
    external_libs/imgui/imgui.cpp
    external_libs/imgui/imgui.h
    external_libs/imgui/imgui_demo.cpp
    external_libs/imgui/imgui_draw.cpp
    external_libs/imgui/imgui_internal.h
    external_libs/imgui/imgui_widgets.cpp
    external_libs/imgui/imgui_tables.cpp
    external_libs/imgui/imstb_rectpack.h
    external_libs/imgui/imstb_textedit.h
    external_libs/imgui/imstb_truetype.h
    external_libs/imgui/backends/imgui_impl_sdlrenderer2.cpp
    external_libs/imgui/backends/imgui_impl_sdlrenderer2.h
    external_libs/imgui/backends/imgui_impl_sdl2.cpp
    external_libs/imgui/backends/imgui_impl_sdl2.h
    external_libs/implot/implot.h
    external_libs/implot/implot.cpp
    external_libs/implot/implot_internal.h
    external_libs/implot/implot_items.cpp
)

include_directories(include ${SDL2_INCLUDE_DIRS} external_libs/imgui external_libs/implot)

add_subdirectory(src)
set (EXTRA_LIBS ${EXTRA_LIBS} Game)

add_executable(Particles main.cpp ${sources} )
target_link_libraries(Particles ${SDL2_LIBRARIES} ${EXTRA_LIBS})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
