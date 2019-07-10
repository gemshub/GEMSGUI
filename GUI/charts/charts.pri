#cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/home/svd/Qt/5.9/gcc_64
#./install-thirdparty.sh /home/svd/Qt/5.9/gcc_64

SOURCES += \
    $$PLOTS_DIR/graph_data.cpp \
    $$PLOTS_DIR/SymbolDialog.cpp \
    $$PLOTS_DIR/LegendDialog.cpp \
    $$PLOTS_DIR/GraphDialog.cpp \
    $$PLOTS_DIR/markershapes.cpp \
    $$PLOTS_DIR/plot_model.cpp \
    $$PLOTS_DIR/chart_view.cpp 
    

HEADERS += \
    $$PLOTS_DIR/graph_data.h \
    $$PLOTS_DIR/GraphDialog.h \
    $$PLOTS_DIR/LegendDialog.h \
    $$PLOTS_DIR/SymbolDialog.h \
    $$PLOTS_DIR/markershapes.h \
    $$PLOTS_DIR/plot_model.h \
    $$PLOTS_DIR/chart_view.h

FORMS += \
    $$PLOTS_DIR/GraphDialog4.ui \
    $$PLOTS_DIR/LegendDialog4.ui \
    $$PLOTS_DIR/SymbolDialog4.ui 
    