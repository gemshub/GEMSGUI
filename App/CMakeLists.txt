
qt_add_executable(gemsgui-app  main.cpp)

set_target_properties(gemsgui-app PROPERTIES
    WIN32_EXECUTABLE TRUE
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

find_library(ChemicalFun_PATH  NAMES ChemicalFun)
if(ChemicalFun_PATH)
    message(STATUS "Found: ${ChemicalFun_PATH}")
    file(COPY ${ChemicalFun_PATH} DESTINATION ${CMAKE_INSTALL_PREFIX}/tmp)
    install(FILES ${ChemicalFun_PATH} DESTINATION tmp2)
else()
    message(STATUS "ChemicalFun not found")
endif()

find_library(ThermoFun_PATH  NAMES ThermoFun)
if(ThermoFun_PATH)
    message(STATUS "Found: ${ThermoFun_PATH}")
    file(COPY ${ThermoFun_PATH} DESTINATION ${CMAKE_INSTALL_PREFIX}/tmp)
    install(FILES ${ThermoFun_PATH} DESTINATION tmp2)
else()
    message(STATUS "ThermoFun not found")
endif()

find_library(fmt_PATH  NAMES fmt)
if(fmt_PATH)
    message(STATUS "Found: ${fmt_PATH}")
    file(COPY ${fmt_PATH} DESTINATION ${CMAKE_INSTALL_PREFIX}/tmp)
    install(FILES ${fmt_PATH} DESTINATION tmp2)
else()
    message(STATUS "fmt not found")
endif()

find_library(zlib_PATH  NAMES zlib)
if(zlib_PATH)
    message(STATUS "Found: ${zlib_PATH}")
    file(COPY ${zlib_PATH} DESTINATION ${CMAKE_INSTALL_PREFIX}/tmp)
    install(FILES ${zlib_PATH} DESTINATION tmp2)
else()
    message(STATUS "zlib not found")
endif()

find_library(libzmq_PATH  NAMES libzmq)
if(libzmq_PATH)
    message(STATUS "Found: ${libzmq_PATH}")
    file(COPY ${libzmq_PATH} DESTINATION ${CMAKE_INSTALL_PREFIX}/tmp)
    install(FILES ${libzmq_PATH} DESTINATION tmp2)
else()
    message(STATUS "libzmq not found")
endif()

if(USE_SPDLOG_PRECOMPILED)
    set(dll_path "C:/Miniconda/envs/GEMSGUI/Library/bin")
    message(STATUS "dll path: ${dll_path}")
    message(STATUS "conda path: $ENV{CONDA_PREFIX}")
    install(FILES "${dll_path}/ChemicalFun.dll" DESTINATION bin)
    install(FILES "${dll_path}/ThermoFun.dll" DESTINATION bin)
    install(FILES "${dll_path}/fmt.dll" DESTINATION bin)
    install(FILES "${dll_path}/libzmq-mt-4_3_5.dll" DESTINATION bin)
    install(FILES "${dll_path}/zlib.dll" DESTINATION bin)

    install(FILES "${dll_path}/gssapi64.dll" DESTINATION bin)
    install(FILES "${dll_path}/libsodium.dll" DESTINATION bin)
    install(FILES "${dll_path}/spdlog.dll" DESTINATION bin)
    install(FILES "${dll_path}/libpng16.dll" DESTINATION bin)

endif()

#find_path(CHEMICALFUN_LIB_DIR NAMES libChemicalFun.dll)
#message(STATUS "ChemicalFun: ${CHEMICALFUN_LIB_DIR}/libChemicalFun.so")
#install(FILES "${CHEMICALFUN_LIB_DIR}/libChemicalFun.so" DESTINATION tmp2)

#find_path(ZLIB_LIB_DIR NAMES libfmt1.so PATHS "/usr")
#message(STATUS "Zlib: ${ZLIB_LIB_DIR}/fmt.so")
#install(FILES "${ZLIB_LIB_DIR}/fmt.so" DESTINATION tmp3)

endif()

qt_generate_deploy_app_script(
    TARGET gemsgui-app
    OUTPUT_SCRIPT deploy_script
    NO_TRANSLATIONS
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
