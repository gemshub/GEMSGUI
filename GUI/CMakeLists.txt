# Recursively collect all header files from the current directory
file(GLOB_RECURSE HEADER_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.hpp *.h)

# Recursively collect all source files from the current directory
file(GLOB_RECURSE SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

# Set some necessary definitions
add_definitions(-DNODEARRAYLEVEL)
add_definitions(-DNO_JSONIO)

# The name of the shared and static libraries
#set(GEMS3KGUI_SHARED_LIB ${PROJECT_NAME}${SUFFIX_SHARED_LIBS})
#set(GEMS3KGUI_STATIC_LIB ${PROJECT_NAME}${SUFFIX_STATIC_LIBS})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


# Enable automatic creation of a module definition (.def) file for a SHARED library on Windows.
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

file(GLOB QRC_FILES *.qrc)
set(RESOURCE ${QRC_FILES})
qt5_add_resources(GEMS3KGUI_SRCS ${RESOURCE})

# Create a library using the collected source files
add_library( GEMS3KGUI_OBJECT OBJECT ${HEADER_FILES} ${SOURCE_FILES} ${GEMS3KGUI_SRCS})

# Check if a shared library is to be built
if(BUILD_SHARED_LIBS)
    add_library(GEMS3KGUI_SHARED SHARED $<TARGET_OBJECTS:GEMS3KGUI_OBJECT>)
#if( ${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    target_link_libraries(GEMS3KGUI_SHARED zmq GEMS3K )
#endif()
    set_target_properties(GEMS3KGUI_SHARED PROPERTIES OUTPUT_NAME gems3kgui )
    install(TARGETS GEMS3KGUI_SHARED 
    EXPORT GEMS3KTargets DESTINATION "lib" COMPONENT libraries)
endif()


# Check if a static library is to be built
if(BUILD_STATIC_LIBS)
   add_library(GEMS3KGUI_STATIC STATIC $<TARGET_OBJECTS:GEMS3KGUI_OBJECT>)
   set_target_properties(GEMS3KGUI_STATIC PROPERTIES OUTPUT_NAME gems3kgui-static)
   install(TARGETS GEMS3KGUI_STATIC 
   EXPORT GEMS3KTargets DESTINATION "lib" COMPONENT libraries)
endif()

## Install debug symbols
#if(MSVC)
#    install(
#        FILES $<TARGET_PDB_FILE:GEMS3KMODULES>
#        DESTINATION ${CMAKE_INSTALL_BINDIR}
#        COMPONENT libraries
#        OPTIONAL
#    )
#endif()

# Install GEMS3K header files
#install(DIRECTORY ${PROJECT_SOURCE_DIR}/GEMS3K
#    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT headers
#    FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")


