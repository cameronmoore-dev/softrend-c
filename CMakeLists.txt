cmake_minimum_required(VERSION 3.28)

project(softrend-c LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_C_EXTENSIONS False)

add_executable(softrend-c
    src/app/entry.c
    src/app/app_context.c
    src/app/scene_manager.c

    src/app/shaders/shd_triangle.c

    src/app/scenes/scn_triangle.c
    src/app/scenes/scn_cube.c

    src/renderer/renderer.c
    src/renderer/shader.c
    src/renderer/_math.c
    src/renderer/colour.c
    
    src/data_structures/stack.c
)

if(WIN32)
    target_sources(softrend-c PUBLIC
        src/os/win32/win32_window.c
    )
elseif(LINUX)
    target_sources(softrend-c PUBLIC
        src/os/linux/x11_window.c
    )
    target_link_libraries(softrend-c PRIVATE -lX11 -lX11-xcb -lxcb -ludev -levdev -lm)
endif()

target_include_directories(softrend-c PRIVATE src)
