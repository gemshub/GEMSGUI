# The path where cmake config files are installed
set(GEMSGUI_INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/GEMSGUI)

install(EXPORT GEMSGUITargets
    FILE GEMSGUITargets.cmake
    NAMESPACE GEMSGUI::
    DESTINATION ${GEMSGUI_INSTALL_CONFIGDIR}
    COMPONENT cmake)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/GEMSGUIConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/GEMSGUIConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/GEMSGUIConfig.cmake
    INSTALL_DESTINATION ${GEMSGUI_INSTALL_CONFIGDIR}
    PATH_VARS GEMSGUI_INSTALL_CONFIGDIR)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/GEMSGUIConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/GEMSGUIConfigVersion.cmake
    DESTINATION ${GEMSGUI_INSTALL_CONFIGDIR})
