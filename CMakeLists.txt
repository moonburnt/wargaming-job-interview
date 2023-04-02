cmake_minimum_required(VERSION 3.21)

project(editor
    LANGUAGES CXX
    VERSION 0.1
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Prefer new GLVND over OLD
set(OpenGL_GL_PREFERENCE GLVND)

add_executable(editor)

target_sources(editor PRIVATE
    src/err_logger.cpp
    src/err_logger.hpp
    src/view.cpp
    src/view.hpp
    src/validators.cpp
    src/validators.hpp
    src/properties.cpp
    src/properties.hpp
    src/objects.cpp
    src/objects.hpp
    src/menu.cpp
    src/menu.hpp
    src/window.cpp
    src/window.hpp
    src/main.cpp
)

target_compile_options(editor PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Wall -Wno-unused -Wextra -Wpedantic -Werror -Wextra-semi -Wsuggest-override -Wno-missing-field-initializers>
    $<$<CXX_COMPILER_ID:MSVC>:/Wall /w34263 /w34266>
    $<$<AND:$<CONFIG:Debug>,$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>>:-fsanitize=address>
)

target_link_options(editor PRIVATE
    $<$<AND:$<CONFIG:Debug>,$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>>:-fsanitize=address>)

target_include_directories(editor PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
)

# Setup spdlog
add_subdirectory("${PROJECT_SOURCE_DIR}/dependencies/spdlog")
target_link_libraries(editor spdlog)
target_include_directories(editor PUBLIC ${spdlog_INCLUDE_DIRS})

# Setup raylib
add_subdirectory("${CMAKE_SOURCE_DIR}/dependencies/raylib")
target_link_libraries(editor raylib)

#setup imgui
add_subdirectory("${PROJECT_SOURCE_DIR}/dependencies/imgui")
target_link_libraries(editor imgui)

# Setup rlimgui
add_subdirectory("${PROJECT_SOURCE_DIR}/dependencies/rlimgui")
target_link_libraries(editor rlimgui)

# Setup nlohmann_json
add_subdirectory("${CMAKE_SOURCE_DIR}/dependencies/nlohmann_json")
set_target_properties(nlohmann_json PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(editor nlohmann_json::nlohmann_json)
