# Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

cmake_minimum_required(VERSION 3.29)

file(GLOB SOURCE_FILES "./*.cpp")
add_library(LIEMidi STATIC ${SOURCE_FILES})
target_include_directories(LIEMidi PUBLIC "./")
target_link_libraries(LIEMidi PUBLIC IECore)
target_link_libraries(LIEMidi PUBLIC IEActions)
target_link_libraries(LIEMidi PUBLIC rtmidi)
target_link_libraries(LIEMidi PUBLIC ryml)
