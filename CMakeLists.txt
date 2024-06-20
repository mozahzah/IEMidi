cmake_minimum_required(VERSION 3.29)
project(IEMidiMapper VERSION 1.0.0 LANGUAGES CXX)

# misc function definitions
function(begin_section_message section_title)
  message("\n____________________________________________________________________________________________________\n")
  message("${section_title}\n")
endfunction()

# Setup output bir
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Setup compiler
set(CMAKE_CXX_STANDARD 20)
begin_section_message("Compiler")
message("Using Compiler: ${CMAKE_CXX_COMPILER_ID}")
message("version: ${CMAKE_CXX_COMPILER_VERSION}")
message("CXX standard: ${CMAKE_CXX_STANDARD}")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

# Misc conditional setup
begin_section_message("Working environment")
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
  include("$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
elseif(CMAKE_SYSTEM_NAME MATCHES "Apple")
    # Apple requirement
endif()
message("System name: ${CMAKE_SYSTEM_NAME}")
message("System version: ${CMAKE_SYSTEM_VERSION}")
message("System processor: ${CMAKE_SYSTEM_PROCESSOR}")

# Add additional subdirectories
begin_section_message("RtMidi")
set(RTMIDI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/rtmidi")
if (EXISTS "${RTMIDI_DIR}/CMakeLists.txt")
  message("CMakeLists.txt found in ${RTMIDI_DIR}")
else()
  message(ERROR "CMakeLists.txt not found in ${RTMIDI_DIR}")
endif()
add_subdirectory(ThirdParty/rtmidi)
begin_section_message("ImGui")
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui")
if (EXISTS "${IMGUI_DIR}/CMakeLists.txt")
  message("CMakeLists.txt found in ${IMGUI_DIR}")
else()
  message(ERROR "CMakeLists.txt not found in ${IMGUI_DIR}")
endif()
add_subdirectory(ThirdParty/imgui)

# Add executable and link libraries
begin_section_message("Adding executable and finalizing library linking")
file(GLOB SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp")
add_library(LIEMidiMapper STATIC ${SOURCE_FILES})
target_include_directories(LIEMidiMapper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include)
target_link_libraries(LIEMidiMapper PUBLIC rtmidi)
target_link_libraries(LIEMidiMapper PUBLIC imgui)

add_executable(EIEMidiMapper Source/main.cpp)
target_link_libraries(EIEMidiMapper PUBLIC LIEMidiMapper)