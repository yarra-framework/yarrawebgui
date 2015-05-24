TARGET = YarraWebGUI

# Define identifier for Ubuntu Linux version (UBUNTU_1204 / UBUNTU_1404)
QMAKE_CXXFLAGS += -DUBUNTU_1204
QMAKE_CXXFLAGS += -DNDEBUG -std=c++0x -DWT_DEPRECATED_3_0_0

LIBS += -lwt -lwthttp -lboost_signals -lboost_system -lboost_filesystem

TEMPLATE = app

SOURCES += main.cpp \
    yw_configuration.cpp \
    yw_application.cpp \
    yw_loginpage.cpp \
    yw_statuspage.cpp \
    yw_configpage.cpp \
    yw_serverinterface.cpp \
    yw_logpage.cpp \
    yw_queuepage.cpp \
    yw_helper.cpp \
    yw_configpage_ymgenerator.cpp

HEADERS += \
    yw_configuration.h \
    yw_application.h \
    yw_loginpage.h \
    yw_global.h \
    yw_statuspage.h \
    yw_configpage.h \
    yw_serverinterface.h \
    yw_logpage.h \
    yw_queuepage.h \
    yw_helper.h \
    yw_configpage_ymgenerator.h

