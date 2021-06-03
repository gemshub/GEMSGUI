# Require a certain version of cmake
cmake_minimum_required(VERSION 3.7.0 )

# Set the name of the project
project(GEMSGUI VERSION 4.0.0 LANGUAGES CXX)

# Set the cmake module path of the project
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

# Use ccache to speed up repeated compilations
#include(CCache)

# Define variables with the GNU standard installation directories
include(GNUInstallDirs)

# Set the cmake module path of the project
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

# Use ccache to speed up repeated compilations
include(CCache)

# Ensure proper configuration if in a conda environment
include(CondaAware)

# Define which types of libraries to build
option(BUILD_SHARED_LIBS "Build shared libraries." ON)
option(BUILD_STATIC_LIBS "Build static libraries." ON)
option(BUILD_APP "Build application." ON)
option(USE_QT6 "Prefer Qt6 over Qt5 if available" OFF)

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
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the list of compiler flags for GNU compiler
if(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    add_compile_options(-std=c++17 -Wall -Wno-pedantic -Wno-variadic-macros -Wno-deprecated)
endif()

# Set the list of compiler flags for Clang compiler
if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    add_compile_options(-std=c++17 -Wall -Wno-ignored-attributes -Wno-pedantic -Wno-variadic-macros -Wno-deprecated)
endif()

# Set the list of compiler flags for Intel compiler
if(${CMAKE_CXX_COMPILER_ID} MATCHES "Intel")
    add_compile_options(-std=c++17 -Wall -Wno-variadic-macros -Wno-deprecated)
endif()

# Set the list of compiler flags for MSVC compiler
if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    add_compile_options(
        /D_SCL_SECURE_NO_WARNINGS
        /D_CRT_SECURE_NO_WARNINGS=1
        /MP4
        /EHsc
        /D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
        /DNOMINMAX
    )
endif()

# Set the source directory path
set(GEMSGUI_MODULES_SOURCE_DIR ${CMAKE_SOURCE_DIR}/Modules)
set(GEMSGUI_GUI_SOURCE_DIR ${CMAKE_SOURCE_DIR}/GUI)

# Set the include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${GEMSGUI_MODULES_SOURCE_DIR})
include_directories(${GEMSGUI_MODULES_SOURCE_DIR}/Submods)
include_directories(${GEMSGUI_MODULES_SOURCE_DIR}/Numerics)
include_directories(${GEMSGUI_GUI_SOURCE_DIR})
include_directories(${GEMSGUI_GUI_SOURCE_DIR}/charts)
include_directories(${GEMSGUI_GUI_SOURCE_DIR}/Services4)
include_directories(${GEMSGUI_GUI_SOURCE_DIR}/Dialogs4)
include_directories(${GEMSGUI_GUI_SOURCE_DIR}/Dataman)
#include_directories("/usr/local/include/GEMS3K")


if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    include_directories("/usr/local/include")
    link_directories("/usr/local/lib")
endif()

# Find all GEMSGUI dependencies
include(GEMSGUIFindDeps)


# Build libraries
add_subdirectory(Modules)
add_subdirectory(GUI)

# make application
if(BUILD_APP)
   add_subdirectory(App)
endif()


# Copy Resources folder
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Resources
#        DESTINATION ${CMAKE_BINARY_DIR})


# Install the cmake config files that permit users to use find_package(ThermoFunGui)
#include(ThermoFunGuiInstallCMakeConfigFiles)
