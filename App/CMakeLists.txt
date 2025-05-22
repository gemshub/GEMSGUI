
qt_add_executable(gemsgui-app  main.cpp)

set_target_properties(gemsgui-app PROPERTIES
    # WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
  target_link_libraries (gemsgui-app PRIVATE GEMSGUI-static libzmq GEMS3K::GEMS3K-static)
else()
  target_link_libraries (gemsgui-app PRIVATE GEMSGUI)
endif()

install(TARGETS gemsgui-app
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)


if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)

if(USE_SPDLOG_PRECOMPILED)
    set(dll_path "C:/Miniconda/envs/GEMSGUI/Library/bin")
    message(STATUS "dll path: ${dll_path}")
    message(STATUS "conda path: $ENV{CONDA_PREFIX}")
    install(FILES "${dll_path}/GEMS3K.dll" DESTINATION bin)
    install(FILES "${dll_path}/ChemicalFun.dll" DESTINATION bin)
    install(FILES "${dll_path}/ThermoFun.dll" DESTINATION bin)
    install(FILES "${dll_path}/fmt.dll" DESTINATION bin)
    install(FILES "${dll_path}/libzmq-mt-4_3_5.dll" DESTINATION bin)
    install(FILES "${dll_path}/zlib.dll" DESTINATION bin)

    install(FILES "${dll_path}/gssapi64.dll" DESTINATION bin)
    install(FILES "${dll_path}/libsodium.dll" DESTINATION bin)
    install(FILES "${dll_path}/spdlog.dll" DESTINATION bin)
    install(FILES "${dll_path}/libpng16.dll" DESTINATION bin)

    install(FILES "${dll_path}/double-conversion.dll" DESTINATION bin)
    install(FILES "${dll_path}/pcre2-16.dll" DESTINATION bin)
    install(FILES "${dll_path}/freetype.dll" DESTINATION bin)
    install(FILES "${dll_path}/zstd.dll" DESTINATION bin)

    install(FILES "${dll_path}/krb5_64.dll" DESTINATION bin)
    install(FILES "${dll_path}/comerr64.dll" DESTINATION bin)
    install(FILES "${dll_path}/k5sprt64.dll" DESTINATION bin)

    install(FILES "${dll_path}/vcruntime140.dll" DESTINATION bin)
    install(FILES "${dll_path}/msvcp140.dll" DESTINATION bin)
    install(FILES "${dll_path}/opengl32sw.dll" DESTINATION bin)

    file(COPY "${CMAKE_SOURCE_DIR}/Resources" DESTINATION bin)
endif()

endif()

qt_generate_deploy_app_script(
    TARGET gemsgui-app
    OUTPUT_SCRIPT deploy_script
    NO_TRANSLATIONS
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
