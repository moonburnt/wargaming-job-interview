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
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
    "$<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
)

# Setup raylib
target_link_libraries(rlimgui raylib)

# Setup imgui
target_link_libraries(rlimgui imgui)
