# SPDX-License-Identifier: GPL-2.0-only
# Copyright © 2024 Interactive Echoes. All rights reserved.
# Author: mozahzah

cmake_minimum_required(VERSION 3.29)

if(WIN32)
  add_executable(${PROJECT_NAME} WIN32 "./main.cpp")
  set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
elseif(APPLE)
  add_executable(${PROJECT_NAME} MACOSX_BUNDLE "./main.cpp")
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND codesign --force --deep -s "-" $<TARGET_BUNDLE_DIR:${PROJECT_NAME}>
    COMMAND codesign --verify --verbose $<TARGET_BUNDLE_DIR:${PROJECT_NAME}>)
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC LIEMidi)

message("Installation prefix: ${CMAKE_INSTALL_PREFIX}")
install(TARGETS ${PROJECT_NAME} 
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

include(InstallRequiredSystemLibraries)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/Resources" DESTINATION ".")
install(DIRECTORY "${CMAKE_SOURCE_DIR}/Requirements" DESTINATION ".")

get_target_property(DEPENDENCIES ${PROJECT_NAME} LINK_LIBRARIES)
foreach(DEPENDENCY ${DEPENDENCIES})
  if(TARGET ${DEPENDENCY})
    message("Installing dependency: ${DEPENDENCY}")
    install(TARGETS ${DEPENDENCY}
        EXPORT "${PROJECT_NAME}Targets"
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
  endif()
endforeach()

begin_section_message("Packaging IEMidi")
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "Interactive Echoes")
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_SOURCE_DIR}/package")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Midi Map Editor Tool")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_VERBATIM_VARIABLES TRUE)
set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}\\Resources\\Icons\\IEIcon.ico")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")

if(WIN32)
  set(CPACK_GENERATOR NSIS64)
  set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}\\Resources\\Icons\\IEIcon.ico")
  set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}\\Resources\\Icons\\IEIcon-Uninstall.ico")
  set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL TRUE)
  set(CPACK_NSIS_MUI_FINISHPAGE_RUN "${CPACK_PACKAGE_NAME}.exe")
  set(CPACK_NSIS_DEFINES "RequestExecutionLevel admin")
  get_filename_component(VULKAN_FILE_NAME ${Vulkan_LIBRARY} NAME_WE)
  set(VULKAN_RT_FILE_PATH "$ENV{WINDIR}\\System32\\${VULKAN_FILE_NAME}.dll")
  message("Expected Vulkan RT ${VULKAN_RT_FILE_PATH}")
  set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "

    DetailPrint \"${VULKAN_RT_FILE_PATH}\"
    IfFileExists \"${VULKAN_RT_FILE_PATH}\" vulkanRT_found vulkanRT_not_found

    vulkanRT_found:
    DetailPrint \"VulkanRT found and installed.\"
    goto post_vulkanRT
    
    vulkanRT_not_found:
    MessageBox MB_YESNO \"VulkanRT must be installed for IEMidi to work, do you want to install VulkanRT now?\" IDYES install_vulkanRT IDNO post_vulkanRT

    install_vulkanRT:
    ExecWait \"\$INSTDIR\\Requirements\\VulkanRT-1.3.290.0-Installer.exe\"

    post_vulkanRT:

    CreateShortCut \"$DESKTOP\\${CPACK_PACKAGE_NAME}.lnk\" \"$INSTDIR\\bin\\${CPACK_PACKAGE_NAME}.exe\" \"\" \"$INSTDIR\\Resources\\Icons\\IEIcon.ico\" 0
    CreateShortCut \"$SMPROGRAMS\\${CPACK_PACKAGE_NAME}.lnk\" \"$INSTDIR\\bin\\${CPACK_PACKAGE_NAME}.exe\" \"\" \"$INSTDIR\\Resources\\Icons\\IEIcon.ico\" 0")
  set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
    Delete \"$DESKTOP\\${CPACK_PACKAGE_NAME}.lnk\"
    Delete \"$SMPROGRAMS\\${CPACK_PACKAGE_NAME}.lnk\"")
elseif(APPLE)
  set(CPACK_GENERATOR DragNDrop)
endif()

include(CPack)