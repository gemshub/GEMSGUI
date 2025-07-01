aux_source_directory(${GEMSGUI_MODULES_SOURCE_DIR} MODULES_SOURCE_FILES)
aux_source_directory(${GEMSGUI_MODULES_SOURCE_DIR}/Submods SUBMODULES_SOURCE_FILES)
aux_source_directory(${GEMSGUI_MODULES_SOURCE_DIR}/Numerics NUMERICS_SOURCE_FILES)

#message(STATUS "Building test suite: ${MODULES_SOURCE_FILES}")

# Recursively collect all header files from the current directory
file(GLOB_RECURSE HEADER_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.hpp *.h)

# Recursively collect all source files from the current directory
file(GLOB_RECURSE SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

file(GLOB QRC_FILES *.qrc)
qt_add_resources(GEMSGUI_SRCS ${QRC_FILES})

# Check if a shared library is to be built
if(BUILD_SHARED_LIBS)
   # Enable automatic creation of a module definition (.def) file for a SHARED library on Windows.
   set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

    qt_add_library(GEMSGUI SHARED
        ${MODULES_SOURCE_FILES} ${SUBMODULES_SOURCE_FILES} ${NUMERICS_SOURCE_FILES}
        ${HEADER_FILES} ${SOURCE_FILES}
        ${GEMSGUI_SRCS}
        )
    target_link_libraries(GEMSGUI PUBLIC zmq GEMS3K::GEMS3K)
    target_link_libraries(GEMSGUI PUBLIC Qt6::Widgets Qt6::Core Qt6::Gui Qt6::PrintSupport Qt6::Charts Qt6::Svg Qt6::Help Qt6::Sql)

endif()

# Check if a static library is to be built
if(BUILD_STATIC_LIBS)

   qt_add_library(GEMSGUI-static STATIC
       ${MODULES_SOURCE_FILES} ${SUBMODULES_SOURCE_FILES} ${NUMERICS_SOURCE_FILES}
       ${HEADER_FILES} ${SOURCE_FILES}
       ${GEMSGUI_SRCS}
   )
   target_link_libraries(GEMSGUI-static PUBLIC libzmq GEMS3K::GEMS3K)
   target_link_libraries(GEMSGUI-static PUBLIC Qt6::Widgets Qt6::Core Qt6::Gui Qt6::PrintSupport Qt6::Charts Qt6::Svg Qt6::Help Qt6::Sql)
   if(USE_SPDLOG_PRECOMPILED)
       target_compile_definitions(GEMSGUI-static PUBLIC USE_SPDLOG_PRECOMPILED)
       target_link_libraries(GEMSGUI-static PUBLIC spdlog::spdlog )
   endif()
endif()
