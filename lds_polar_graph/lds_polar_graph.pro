QT += charts

HEADERS += \
    lds_polar_graph.h

SOURCES += \
    main.cpp \
    lds_polar_graph.cpp

unix {
#    LIBS += -lboost_system
}
