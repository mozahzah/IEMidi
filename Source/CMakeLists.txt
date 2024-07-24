# SPDX-License-Identifier: GPL-2.0-only
# Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

cmake_minimum_required(VERSION 3.29)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
file(GLOB SOURCE_FILES "./*.cpp")
add_library(LIEMidi SHARED ${SOURCE_FILES})
target_include_directories(LIEMidi PUBLIC "./")
file(GLOB HEADER_FILES "./*.h")
set_property(TARGET LIEMidi PROPERTY PUBLIC_HEADER ${HEADER_FILES})
target_link_libraries(LIEMidi PUBLIC IECore)
target_link_libraries(LIEMidi PUBLIC IEActions)
target_link_libraries(LIEMidi PUBLIC rtmidi)
target_link_libraries(LIEMidi PUBLIC ryml)
