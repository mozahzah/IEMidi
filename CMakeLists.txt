cmake_minimum_required(VERSION 3.24)
project(IEMidiMapper VERSION 1.0.0 LANGUAGES CXX)

# Output and compiler setup
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Compiler setup
set(CMAKE_CXX_STANDARD 20)
message("Using " ${CMAKE_CXX_COMPILER_ID})

add_executable(IEMidiMapper Source/main.cpp)
add_subdirectory(ThirdParty/rtmidi)
add_subdirectory(ThirdParty/imgui)

# Library linking
target_link_libraries(IEMidiMapper PUBLIC rtmidi)
target_link_libraries(IEMidiMapper PUBLIC imgui)