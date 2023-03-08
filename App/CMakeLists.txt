LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/Modules)
LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/GUI)


# Create an executable named app with source code main.cpp
add_executable(gemsgui-app main.cpp)

# Link your application against ThermoFun C++ library.
target_link_libraries (gemsgui-app GEMSGUI gems3kmodules ${GEMS_QT_TARGETS} zmq GEMS3K::GEMS3K)

add_dependencies(gemsgui-app GEMS3KMODULES_SHARED)
add_dependencies(gemsgui-app GEMSGUI_SHARED)
