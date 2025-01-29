cmake_minimum_required(VERSION 3.10.0)
project(hello_window VERSION 1.0.0)

find_package(OpenGL REQUIRED)

file(GLOB IMGUI_SOURCES "include/imgui/*.cpp" "include/imgui/backends/imgui_impl_glfw.cpp" "include/imgui/backends/imgui_impl_opengl3.cpp")

add_executable(hello_window src/main.cpp ${IMGUI_SOURCES})

target_compile_definitions(hello_window PRIVATE GLEW_STATIC)

target_include_directories(hello_window PRIVATE include include/imgui)

target_link_directories(hello_window PRIVATE lib/GLFW lib/GL)

target_link_libraries(hello_window PRIVATE libglfw3.a glew32s.lib OpenGL::GL)