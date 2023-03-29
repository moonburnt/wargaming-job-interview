cmake_minimum_required(VERSION 3.21)

# set project name and version
project(rlimgui
    LANGUAGES CXX
    VERSION 0.1
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(rlimgui STATIC
    rlImGui.cpp
    rlImGui.h
    rlImGuiColors.h
)

target_include_directories(rlimgui PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
    "$<INSTALL_INTERFACE:${PROJECT_SOURCE_DIR}>"
)

# Setup raylib
add_subdirectory("${CMAKE_SOURCE_DIR}/dependencies/raylib" "${CMAKE_SOURCE_DIR}/build/rlimgui")
target_link_libraries(rlimgui raylib)
target_include_directories(rlimgui PUBLIC ${raylib_INCLUDE_DIRS})

# Setup imgui
add_subdirectory("${CMAKE_SOURCE_DIR}/dependencies/imgui" "${CMAKE_SOURCE_DIR}/build/imgui")
target_link_libraries(rlimgui imgui)
target_include_directories(rlimgui PUBLIC ${imgui_INCLUDE_DIRS})
