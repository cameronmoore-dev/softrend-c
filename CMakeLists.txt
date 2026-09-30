cmake_minimum_required(VERSION 3.28)

project(softrend-c LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_C_EXTENSIONS False)

add_executable(softrend-c
    src/app/entry.c
)

if(WIN32)
    target_sources(softrend-c PUBLIC
        src/window/win32/win32_window.c
    )
elseif(Linux)

endif()
target_include_directories(softrend-c PRIVATE src)
