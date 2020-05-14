find_package(Qt5 COMPONENTS Core Gui Widgets Charts Concurrent PrintSupport Svg Help Sql REQUIRED)
include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Gui_INCLUDE_DIRS})
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5PrintSupport_INCLUDE_DIRS})
include_directories(${Qt5Svg_INCLUDE_DIRS})


#find_package(GEMS3K REQUIRED)

#find_package(ZeroMQ REQUIRED)

