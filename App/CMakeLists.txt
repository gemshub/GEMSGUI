
LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/Modules)
LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/GUI)
# Set some necessary definitions
add_definitions(-DNODEARRAYLEVEL)
add_definitions(-DNO_JSONIO)

# Create an executable named app with source code main.cpp
add_executable(gems3kgui-app main.cpp)

# Link your application against ThermoFun C++ library.
target_link_libraries (gems3kgui-app gems3kgui gems3kmodules Qt5::Widgets Qt5::Core Qt5::Gui Qt5::PrintSupport Qt5::Charts Qt5::Svg Qt5::Help zmq GEMS3K )


