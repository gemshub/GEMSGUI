# **gems3k-export** the Qt console application for export systems from the internal database projects.
message(STATUS "Generatethe Qt console application for export systems")
add_definitions(-DNO_GUI)

aux_source_directory(${GEMSGUI_MODULES_SOURCE_DIR} MODULES_SOURCE_FILES1)
aux_source_directory(${GEMSGUI_MODULES_SOURCE_DIR}/Submods SUBMODULES_SOURCE_FILES1)
aux_source_directory(${GEMSGUI_MODULES_SOURCE_DIR}/Numerics NUMERICS_SOURCE_FILES1)
aux_source_directory(${GEMSGUI_GUI_SOURCE_DIR}/Dataman DATAMAN_SOURCE_FILES1)

#message(STATUS "Building test suite: ${MODULES_SOURCE_FILES}")

# Recursively collect all header files from the current directory
file(GLOB_RECURSE HEADER_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.hpp *.h)

# Recursively collect all source files from the current directory
file(GLOB_RECURSE SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)


set(EXPORT_SOURCES
      ${MODULES_SOURCE_FILES1}
      ${SUBMODULES_SOURCE_FILES1}
      ${NUMERICS_SOURCE_FILES1}
      ${DATAMAN_SOURCE_FILES1}
      ${GEMSGUI_GUI_SOURCE_DIR}/Services4/visor.h
      ${GEMSGUI_GUI_SOURCE_DIR}/Services4/units.h
      ${GEMSGUI_GUI_SOURCE_DIR}/Services4/service.h
      ${GEMSGUI_GUI_SOURCE_DIR}/Services4/graph.h
      ${GEMSGUI_GUI_SOURCE_DIR}/Services4/graph_window.h
      ${GEMSGUI_GUI_SOURCE_DIR}/Services4/visor.cpp
      ${GEMSGUI_GUI_SOURCE_DIR}/Services4/units.cpp
      ${GEMSGUI_GUI_SOURCE_DIR}/Services4/graph.cpp
      ${GEMSGUI_GUI_SOURCE_DIR}/charts/chart_model.h
      ${GEMSGUI_GUI_SOURCE_DIR}/charts/graph_data.h
      ${GEMSGUI_GUI_SOURCE_DIR}/charts/chart_model.cpp
      ${GEMSGUI_GUI_SOURCE_DIR}/charts/graph_data.cpp
      ${HEADER_FILES} ${SOURCE_FILES}
    )

qt_add_executable(gems3k-export ${EXPORT_SOURCES})

if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    target_link_libraries(gems3k-export PRIVATE GEMS3K::GEMS3K-static Qt6::Core)
else()
    target_link_libraries(gems3k-export PRIVATE GEMS3K::GEMS3K Qt6::Core)
endif()

install(TARGETS gems3k-export
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)



