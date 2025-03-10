# Recursively collect all header files from the current directory
file(GLOB_RECURSE HEADER_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.hpp *.h)

# Recursively collect all source files from the current directory
file(GLOB_RECURSE SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

# Check if a shared library is to be built
if(BUILD_SHARED_LIBS)
    # Enable automatic creation of a module definition (.def) file for a SHARED library on Windows.
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

    qt_add_library(gems3kmodules SHARED ${HEADER_FILES} ${SOURCE_FILES})
    target_link_libraries(gems3kmodules PUBLIC GEMS3K::GEMS3K)
    target_link_libraries(gems3kmodules PRIVATE Qt6::Gui Qt6::Widgets)

 endif()

# Check if a static library is to be built
if(BUILD_STATIC_LIBS)
    qt_add_library(gems3kmodules-static STATIC ${HEADER_FILES} ${SOURCE_FILES})
    target_link_libraries(gems3kmodules-static PUBLIC GEMS3K::GEMS3K-static)
    target_link_libraries(gems3kmodules-static PRIVATE Qt6::Gui Qt6::Widgets)

endif()




