# Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

cmake_minimum_required(VERSION 3.29)

if(WIN32)
  add_executable(IEMidi WIN32 "./main.cpp")
  set_target_properties(IEMidi PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
elseif(APPLE)
  add_executable(IEMidi MACOSX_BUNDLE "./main.cpp")
  add_custom_command(TARGET IEMidi POST_BUILD
    COMMAND codesign --force --deep -s "-" $<TARGET_BUNDLE_DIR:IEMidi>
    COMMAND codesign --verify --verbose $<TARGET_BUNDLE_DIR:IEMidi>)
endif()

target_link_libraries(IEMidi PUBLIC LIEMidi)

message("Installation prefix: ${CMAKE_INSTALL_PREFIX}")
install(TARGETS ${PROJECT_NAME} 
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

get_target_property(DEPENDENCIES ${PROJECT_NAME} LINK_LIBRARIES)
foreach(DEPENDENCY ${DEPENDENCIES})
  if(TARGET ${DEPENDENCY})
    message("Installing dependency: ${DEPENDENCY}")
    install(TARGETS ${DEPENDENCY}
        EXPORT "${PROJECT_NAME}Targets"
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})
  endif()
endforeach()

# message("Adding additional source files and variables for ${CMAKE_SYSTEM_NAME}")
# if(CMAKE_SYSTEM_NAME MATCHES "Windows")
#   # Specific windows source files and variables can be appended here
# elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
#   set(MACOSX_BUNDLE TRUE)
#   set(MACOSX_BUNDLE_BUNDLE_NAME "IEMidi")
#   set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.IE.IEMidi")
#   set(MACOSX_BUNDLE_BUNDLE_VERSION "1.0.0")
#   set(MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0")
#   set(CODE_SIGN_IDENTITY "Incus Entertainment")
#   set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "Incus Entertainment")
# endif()

# begin_section_message("Packaging IEMidi")
# set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
# set(CPACK_PACKAGE_VENDOR "Incus Entertainment")
# set(CPACK_PACKAGE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/package")
# file(MAKE_DIRECTORY ${CPACK_PACKAGE_DIRECTORY})
# set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}")
# set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Midi Map Editor Tool")
# set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
# set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
# set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
# set(CPACK_VERBATIM_VARIABLES TRUE)
# set(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}\\Resources\\Icons\\IEIcon.ico")
# set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

# if(CMAKE_SYSTEM_NAME MATCHES "Windows")
# set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}\\Resources\\Icons\\IEIcon.ico")
# set(CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}\\Resources\\Icons\\Uninstall-IEIcon.ico")
# set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL TRUE)
# set(CPACK_NSIS_MUI_FINISHPAGE_RUN "${CPACK_PACKAGE_NAME}.exe")
# set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
#     CreateShortCut \"$DESKTOP\\${CPACK_PACKAGE_NAME}.lnk\" \"$INSTDIR\\bin\\${CPACK_PACKAGE_NAME}.exe\" \"\" \"$INSTDIR\\Resources\\Icons\\IEIcon.ico\" 0
#     CreateShortCut \"$SMPROGRAMS\\${CPACK_PACKAGE_NAME}.lnk\" \"$INSTDIR\\bin\\${CPACK_PACKAGE_NAME}.exe\" \"\" \"$INSTDIR\\Resources\\Icons\\IEIcon.ico\" 0
# ")
# set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
#     Delete \"$DESKTOP\\${CPACK_PACKAGE_NAME}.lnk\"
#     Delete \"$SMPROGRAMS\\${CPACK_PACKAGE_NAME}.lnk\"
# ")
# elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
# endif()

# include(CPack)
# begin_section_message("Finished Setting IEMidi")