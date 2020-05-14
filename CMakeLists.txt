# Require a certain version of cmake
cmake_minimum_required(VERSION 3.7.0 )

# Set the name of the project
project(gems3kgui VERSION 4.0.0 LANGUAGES CXX)

# Set the cmake module path of the project
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

# Use ccache to speed up repeated compilations
#include(CCache)

# Define variables with the GNU standard installation directories
include(GNUInstallDirs)

# Ensure proper configuration if in a conda environment
#include(CondaAware)

# Define which types of libraries to build
option(BUILD_SHARED_LIBS "Build shared libraries." ON)
option(BUILD_STATIC_LIBS "Build static libraries." OFF)

# Set the default build type to Release
if(NOT CMAKE_BUILD_TYPE)
    # The build type selection for the project
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the build type for ${PROJECT_NAME}." FORCE)
    # The build type options for the project
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release MinSizeRel RelWithDebInfo)
endif()

# Define if shared library should be build instead of static.
#option(BUILD_SHARED_LIBS "Build shared libraries." ON)

# Set libraries to be compiled with position independent code mode (i.e., fPIC option in GNU compilers)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the list of compiler flags for GNU compiler
if(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    add_compile_options(-std=c++11 -Wall -Wno-pedantic -Wno-variadic-macros -Wno-deprecated)
endif()

# Set the list of compiler flags for Clang compiler
if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    add_compile_options(-std=c++11 -Wall -Wno-ignored-attributes -Wno-pedantic -Wno-variadic-macros -Wno-deprecated)
endif()

# Set the list of compiler flags for Intel compiler
if(${CMAKE_CXX_COMPILER_ID} MATCHES "Intel")
    add_compile_options(-std=c++11 -Wall -Wno-variadic-macros -Wno-deprecated)
endif()

# Set the list of compiler flags for MSVC compiler
if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    add_compile_options("/W0 -D_SCL_SECURE_NO_WARNINGS /MP4")
endif()

# Set the source directory path
set(GEMS3KGUI_MODULES_SOURCE_DIR ${CMAKE_SOURCE_DIR}/Modules)
set(GEMS3KGUI_GUI_SOURCE_DIR ${CMAKE_SOURCE_DIR}/GUI)

# Set the include directories
include_directories(${GEMS3KGUI_MODULES_SOURCE_DIR})
include_directories(${GEMS3KGUI_MODULES_SOURCE_DIR}/Submods)
include_directories(${GEMS3KGUI_MODULES_SOURCE_DIR}/Numerics)
include_directories(${GEMS3KGUI_GUI_SOURCE_DIR})
include_directories(${GEMS3KGUI_GUI_SOURCE_DIR}/charts)
include_directories(${GEMS3KGUI_GUI_SOURCE_DIR}/Services4)
include_directories(${GEMS3KGUI_GUI_SOURCE_DIR}/Dialogs4)
include_directories(${GEMS3KGUI_GUI_SOURCE_DIR}/Dataman)
include_directories("/usr/local/include/GEMS3K")


if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    include_directories("/usr/local/include")
    link_directories("/usr/local/lib")
endif()

# Find all GEMS3KGui dependencies
include(GEMS3KGuiFindDeps)

#find_package(Qt5 COMPONENTS Core Gui Widgets Charts Concurrent PrintSupport Svg Sql REQUIRED)
#include_directories(${Qt5Core_INCLUDE_DIRS})
#include_directories(${Qt5Gui_INCLUDE_DIRS})
#include_directories(${Qt5Widgets_INCLUDE_DIRS})
#include_directories(${Qt5PrintSupport_INCLUDE_DIRS})
#include_directories(${Qt5Svg_INCLUDE_DIRS})


# Build libraries
add_subdirectory(Modules)
add_subdirectory(GUI)

# make application
add_subdirectory(App)


# Copy Resources folder
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Resources
#        DESTINATION ${CMAKE_BINARY_DIR})


# Install the cmake config files that permit users to use find_package(ThermoFunGui)
#include(ThermoFunGuiInstallCMakeConfigFiles)
