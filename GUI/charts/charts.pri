#cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/home/svd/Qt/5.9/gcc_64
#./install-thirdparty.sh /home/svd/Qt/5.9/gcc_64

SOURCES += \
    $$CHARTS_CPP/graph_data.cpp \
    $$CHARTS_CPP/SymbolDialogN.cpp \
    $$CHARTS_CPP/LegendDialogN.cpp \
    $$CHARTS_CPP/GraphDialogN.cpp \
    $$CHARTS_CPP/markershapes.cpp \
#    $$CHARTS_CPP/plot_model.cpp \
    $$CHARTS_CPP/chart_view.cpp
    

HEADERS += \
    $$CHARTS_H/graph_data.h \
    $$CHARTS_H/GraphDialogN.h \
    $$CHARTS_H/LegendDialogN.h \
    $$CHARTS_H/SymbolDialogN.h \
    $$CHARTS_H/markershapes.h \
    $$CHARTS_H/plot_model.h \
    $$CHARTS_H/chart_view.h \
    $$PWD/from_jsonio.h

FORMS += \
    $$CHARTS_CPP/GraphDialog4N.ui \
    $$CHARTS_CPP/LegendDialog4N.ui \
    $$CHARTS_CPP/SymbolDialog4N.ui
    
