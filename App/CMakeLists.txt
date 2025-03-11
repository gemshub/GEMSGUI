
qt_add_executable(gemsgui-app  main.cpp)

set_target_properties(gemsgui-app PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
  target_link_libraries (gemsgui-app PRIVATE GEMSGUI-static)
  add_dependencies(gemsgui-app GEMSGUI-static)
else()
  target_link_libraries (gemsgui-app PRIVATE GEMSGUI)
  add_dependencies(gemsgui-app GEMSGUI)
endif()

install(TARGETS gemsgui-app
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET gemsgui-app
    OUTPUT_SCRIPT deploy_script
    NO_TRANSLATIONS
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
