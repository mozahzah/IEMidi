cmake_minimum_required(VERSION 3.29)
project(IEMidi VERSION 1.0.0 LANGUAGES CXX)

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
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
begin_section_message("Compiler")
message("Using Compiler: ${CMAKE_CXX_COMPILER_ID}")
message("version: ${CMAKE_CXX_COMPILER_VERSION}")
message("CXX standard: ${CMAKE_CXX_STANDARD}")

# Misc conditional setup
begin_section_message("Working environment")
message("System name: ${CMAKE_SYSTEM_NAME}")
message("System version: ${CMAKE_SYSTEM_VERSION}")
message("System processor: ${CMAKE_SYSTEM_PROCESSOR}")
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
  set(VCPKG_CMAKE_PATH "$ENV{VCPKG_ROOT}\\scripts\\buildsystems\\vcpkg.cmake")
  include(${VCPKG_CMAKE_PATH})
  message("Using vcpkg cmake found at ${VCPKG_CMAKE_PATH}")
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  set(CMAKE_OSX_SYSROOT /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk)
  find_library(COREAUDIO_LIBRARY CoreAudio)
  set_source_files_properties(IERenderer.mm PROPERTIES LANGUAGE CXX)
endif()

# Add additional subdirectories
begin_section_message("RtMidi")
set(RTMIDI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/rtmidi")
if (EXISTS "${RTMIDI_DIR}/CMakeLists.txt")
  message("CMakeLists.txt found in ${RTMIDI_DIR}")
  add_subdirectory(ThirdParty/rtmidi)
else()
  message(ERROR "CMakeLists.txt not found in ${RTMIDI_DIR}")
endif()

begin_section_message("ImGui")
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui")
if (EXISTS "${IMGUI_DIR}/CMakeLists.txt")
  message("CMakeLists.txt found in ${IMGUI_DIR}")
  add_subdirectory(ThirdParty/imgui)
else()
  message(ERROR "CMakeLists.txt not found in ${IMGUI_DIR}")
endif()

begin_section_message("RapidYaml")
set(RAPIDYAML_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/rapidyaml")
if (EXISTS "${RAPIDYAML_DIR}/CMakeLists.txt")
  message("CMakeLists.txt found in ${RAPIDYAML_DIR}")
  add_subdirectory(ThirdParty/rapidyaml)
else()
  message(ERROR "CMakeLists.txt not found in ${RAPIDYAML_DIR}")
endif()

begin_section_message("stb")
set(STB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/stb")
message("Adding ${STB_DIR} as include directory for stb library")
include_directories("${STB_DIR}")

# Add executable and link libraries
begin_section_message("Adding executable and finalizing library linking")
file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp")

message("Adding additional source files and variables for ${CMAKE_SYSTEM_NAME}")
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
  # Specific windows source files and variables can be appended here
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  set(MACOSX_BUNDLE TRUE)
  set(MACOSX_BUNDLE_BUNDLE_NAME "IEMidi")
  set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.IE.IEMidi")
  set(MACOSX_BUNDLE_BUNDLE_VERSION "1.0.0")
  set(MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0")
  set(CODE_SIGN_IDENTITY "Incus Entertainment")
  set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "Incus Entertainment")

  file(GLOB_RECURSE ADD_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.mm")
  list(APPEND SOURCE_FILES ${ADD_SOURCE_FILES})
endif()

add_library(LIEMidi STATIC ${SOURCE_FILES})
target_include_directories(LIEMidi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Include)
target_link_libraries(LIEMidi PUBLIC rtmidi)
target_link_libraries(LIEMidi PUBLIC imgui)
target_link_libraries(LIEMidi PUBLIC ryml)

message("Linking additional libraries for ${CMAKE_SYSTEM_NAME}")
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
  target_link_libraries(LIEMidi PUBLIC dwmapi)
  target_link_libraries(LIEMidi PUBLIC uxtheme)
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  target_link_libraries(LIEMidi PUBLIC ${COREAUDIO_LIBRARY})
  target_link_libraries(LIEMidi PUBLIC "-framework Cocoa")
  target_link_libraries(LIEMidi PUBLIC "-framework UserNotifications")
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
  add_executable(IEMidi WIN32 Source/main.cpp)
  set_target_properties(IEMidi PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  add_executable(IEMidi MACOSX_BUNDLE Source/main.cpp)
  add_custom_command(TARGET IEMidi POST_BUILD
    COMMAND codesign --force --deep -s "-" $<TARGET_BUNDLE_DIR:IEMidi>
    COMMAND codesign --verify --verbose $<TARGET_BUNDLE_DIR:IEMidi>)
endif()

target_link_libraries(IEMidi PUBLIC LIEMidi)
begin_section_message("Finished Setting IEMidi")