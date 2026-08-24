# **gems3k-export** the Qt console application for export systems from the internal database projects.
message(STATUS "Generatethe Qt console application for export systems")

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/template_export.json" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

add_definitions(-DNO_GUI)

aux_source_directory(${GEMSGUI_MODULES_SOURCE_DIR} MODULES_SOURCE_FILES1)
aux_source_directory(${GEMSGUI_MODULES_SOURCE_DIR}/Submods SUBMODULES_SOURCE_FILES1)
aux_source_directory(${GEMSGUI_MODULES_SOURCE_DIR}/Numerics NUMERICS_SOURCE_FILES1)
aux_source_directory(${GEMSGUI_GUI_SOURCE_DIR}/Dataman DATAMAN_SOURCE_FILES1)

#message(STATUS "Building test suite: ${MODULES_SOURCE_FILES}")

# Recursively collect all header files from the current directory
file(GLOB_RECURSE HEADER_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.hpp *.h)

# Sources shared by every console tool built here (NO_GUI stand-ins for the
# vf*/graph-window Qt-Widgets code, plus the Modules/Dataman/Services4 stack).
# Each tool below adds its own single *_main.cpp on top of this list --
# do NOT glob *.cpp here, or multiple mains would end up in one executable.
set(COMMON_SOURCES
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
      ${HEADER_FILES}
      ${CMAKE_CURRENT_SOURCE_DIR}/service_new.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/graph_window_new.cpp
    )

qt_add_executable(gems3k-export ${COMMON_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/export_main.cpp)

# json2db: proof-of-concept CLI that creates a new <keyword>.pdb/.ndx pair
# (icomp/dcomp/phase/reacdc/sdref) from a JSON records file, with no
# GUI/QApplication involved. See json2db_main.cpp.
qt_add_executable(json2db ${COMMON_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/json2db_main.cpp)

foreach(gems_tool gems3k-export json2db)
    if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
        target_link_libraries(${gems_tool} PRIVATE GEMS3K::GEMS3K-static Qt6::Core)
    else()
        target_link_libraries(${gems_tool} PRIVATE GEMS3K::GEMS3K Qt6::Core)
    endif()
endforeach()

#install(TARGETS gems3k-export
#    BUNDLE  DESTINATION .
#    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#)

# json2db needs a copy of Resources/ at runtime (schema/config, via -s) --
# install it next to the binary's prefix so other repos (e.g. a `cmake
# --build --target json2db && cmake --install` step in a Python project's
# own dependency-install script) get a complete, relocatable install.
#
# Only 3 files out of the full ~61MB Resources/ tree are actually read by
# TVisor::Setup() in a NO_GUI/json2db run: the object-schema and units
# caches (data/vis_od.ini.json, data/units.ini.json -- these define every
# field of every keyword: name, type, size) and gemsgui-config.json. None
# of the built-in TDB databases (DB.default/), sample projects (projects/),
# help database (help/), or icons/images are touched -- confirmed by
# actually running json2db against a Resources/ containing only these 3
# files (plus an empty projects/ dir, see below) and nothing else.
install(FILES
    ${CMAKE_SOURCE_DIR}/Resources/data/vis_od.ini.json
    ${CMAKE_SOURCE_DIR}/Resources/data/units.ini.json
    DESTINATION ${CMAKE_INSTALL_DATADIR}/gemsgui/Resources/data
)
install(FILES
    ${CMAKE_SOURCE_DIR}/Resources/gemsgui-config.json
    DESTINATION ${CMAKE_INSTALL_DATADIR}/gemsgui/Resources
)
# TVisor::firstTimeSetup() -- triggered whenever the -u user dir doesn't
# exist yet -- does a raw `cp -r Resources/projects/* <userdir>/projects`
# and treats a *missing* source projects/ dir as a fatal error (it doesn't
# just skip the copy). We ship an (empty) placeholder so a first run against
# this trimmed Resources/ doesn't crash; callers should still prefer
# pre-creating the -u dir themselves (see conda-install-dependencies.sh in
# thermoimpex-jupyter) so firstTimeSetup() never actually runs per-call.
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/empty_projects_placeholder.txt
    DESTINATION ${CMAKE_INSTALL_DATADIR}/gemsgui/Resources/projects
)
install(TARGETS json2db
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)



