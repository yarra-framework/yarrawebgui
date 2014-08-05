QT       += core
QT       -= gui

LIBS += -lwt -lwthttp -lboost_signals
QMAKE_CXXFLAGS += -DNDEBUG -std=c++0x -DWT_DEPRECATED_3_0_0
CONFIG   += console
CONFIG   -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    yw_configuration.cpp \
    yw_application.cpp \
    yw_loginpage.cpp

HEADERS += \
    yw_configuration.h \
    yw_application.h \
    yw_loginpage.h \
    yw_global.h

