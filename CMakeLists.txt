cmake_minimum_required(VERSION 3.25)
project(AttractorRenderer)

set(CMAKE_CXX_STANDARD 23)


add_subdirectory("libs")


add_executable(AttractorRenderer main.cpp Window.h Attractor.h Buffer.h Palette.h vector.h lodepng.cpp lodepng.h libs/ImGuiFileDialog/ImGuiFileDialog.cpp)
#add_executable(AttractorRenderer)
#target_sources(AttractorRenderer PUBLIC ${CMAKE_SOURCE_DIR}/libs/imgui/examples/example_sdl3_opengl3/main.cpp)

target_link_libraries(AttractorRenderer IMGUI)
set_target_properties(AttractorRenderer PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
