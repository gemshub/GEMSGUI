TEMPLATE = app
CONFIG += console c++20
CONFIG -= app_bundle
CONFIG -= qt

DEFINES  += NODEARRAYLEVEL
#DEFINES += USE_NLOHMANNJSON
DEFINES += NDEBUG
#!win32:!macx-clang:DEFINES += OVERFLOW_EXCEPT  #compile with nan inf exceptions

OBJECTS_DIR       = obj

# link lib
INCLUDEPATH   += "/usr/local/include/GEMS3K"
DEPENDPATH   += "/usr/local/include/GEMS3K"
LIBS += -lGEMS3K

#HEADERS	 += config.h

#SOURCES	+=  config.cpp \
#    ini2json.cpp


HEADERS	 += help.h

SOURCES	+=  help.cpp \
    qhp-generator.cpp
