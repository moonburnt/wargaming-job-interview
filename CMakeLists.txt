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
    src/main.cpp
)

target_compile_options(editor PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Wall -Wextra -Wpedantic -Werror -Wextra-semi -Wsuggest-override -Wno-missing-field-initializers>
    $<$<CXX_COMPILER_ID:MSVC>:/Wall /w34263 /w34266>
    $<$<AND:$<CONFIG:Debug>,$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>>:-fsanitize=address>
)

target_link_options(editor PRIVATE
    $<$<AND:$<CONFIG:Debug>,$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>>:-fsanitize=address>)

target_include_directories(editor PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
)

# Setup raylib
add_subdirectory("${PROJECT_SOURCE_DIR}/dependencies/raylib")
target_link_libraries(editor raylib)
target_include_directories(editor PUBLIC ${raylib_INCLUDE_DIRS})

# Setup nlohmann_json
add_subdirectory("${CMAKE_SOURCE_DIR}/dependencies/nlohmann_json")
set_target_properties(nlohmann_json PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(editor nlohmann_json::nlohmann_json)
