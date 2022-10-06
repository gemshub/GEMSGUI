

# Set some necessary definitions
add_definitions(-DNODEARRAYLEVEL)
add_definitions(-DNO_JSONIO)

# The name of the shared and static libraries
#set(GEMSGUI_SHARED_LIB ${PROJECT_NAME}${SUFFIX_SHARED_LIBS})
#set(GEMSGUI_STATIC_LIB ${PROJECT_NAME}${SUFFIX_STATIC_LIBS})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
#set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Recursively collect all header files from the current directory
file(GLOB_RECURSE HEADER_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.hpp *.h)

# Recursively collect all source files from the current directory
file(GLOB_RECURSE SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

# Enable automatic creation of a module definition (.def) file for a SHARED library on Windows.
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

file(GLOB QRC_FILES *.qrc)
set(RESOURCE ${QRC_FILES})
if(Qt6_FOUND)
  qt6_add_resources(GEMSGUI_SRCS ${RESOURCE})
else()
  qt5_add_resources(GEMSGUI_SRCS ${RESOURCE})
endif()

# Create a library using the collected source files
add_library( GEMSGUI_OBJECT OBJECT ${HEADER_FILES} ${SOURCE_FILES} ${GEMSGUI_SRCS})

# Check if a shared library is to be built
if(BUILD_SHARED_LIBS)
   add_library(GEMSGUI_SHARED SHARED $<TARGET_OBJECTS:GEMSGUI_OBJECT>)
#if( ${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    target_link_libraries(GEMSGUI_SHARED zmq GEMS3K )
#endif()
    set_target_properties(GEMSGUI_SHARED PROPERTIES OUTPUT_NAME GEMSGUI )
    install(TARGETS GEMSGUI_SHARED 
    EXPORT GEMSGUIKTargets DESTINATION "lib" COMPONENT libraries)
endif()


# Check if a static library is to be built
if(BUILD_STATIC_LIBS)
   add_library(GEMSGUI_STATIC STATIC $<TARGET_OBJECTS:GEMSGUI_OBJECT>)
   set_target_properties(GEMSGUI_STATIC PROPERTIES OUTPUT_NAME GEMSGUI-static)
   install(TARGETS GEMSGUI_STATIC 
   EXPORT GEMSGUITargets DESTINATION "lib" COMPONENT libraries)
endif()

## Install debug symbols
#if(MSVC)
#    install(
#        FILES $<TARGET_PDB_FILE:GEMSGUIMODULES>
#        DESTINATION ${CMAKE_INSTALL_BINDIR}
#        COMPONENT libraries
#        OPTIONAL
#    )
#endif()

# Install GEMSGUI header files
#install(DIRECTORY ${PROJECT_SOURCE_DIR}/GEMSGUI
#    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT headers
#    FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")


