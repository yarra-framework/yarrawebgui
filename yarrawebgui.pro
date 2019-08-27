TARGET = YarraWebGUI

# Define identifier for Ubuntu Linux version (UBUNTU_1204 / UBUNTU_1404 / UBUNTU_1604)
BUILD_OS=UBUNTU_1604

equals( BUILD_OS, "UBUNTU_1604" ) {
    message( "Configuring for Ubuntu 16.04" )
    QMAKE_CXXFLAGS += -DUBUNTU_1604
    ICU_PATH=/usr/lib/x86_64-linux-gnu
    BOOST_PATH=/usr/lib/x86_64-linux-gnu
}

equals( BUILD_OS, "UBUNTU_1404" ) {
    message( "Configuring for Ubuntu 14.04" )
    QMAKE_CXXFLAGS += -DUBUNTU_1404
    ICU_PATH=/usr/lib/x86_64-linux-gnu
    BOOST_PATH=/usr/lib/x86_64-linux-gnu
}

equals( BUILD_OS, "UBUNTU_1204" ) {
    message( "Configuring for Ubuntu 12.04" )
    QMAKE_CXXFLAGS += -DUBUNTU_1204
    ICU_PATH=/usr/lib
    BOOST_PATH=/usr/local/lib
}

QMAKE_CXXFLAGS += -DNDEBUG -std=c++11 -DWT_DEPRECATED_3_0_0

# For compiling / linking the ziplib
libzip_lib = $$PWD/ziplib/Bin/libzip.a
LIBS += $$libzip_lib

libzip.target = $$libzip_lib
libzip.commands = cd $$PWD/ziplib && make all
libzip.CONFIG += recursive

QMAKE_EXTRA_TARGETS += libzip
PRE_TARGETDEPS += $$libzip_lib
INCLUDEPATH += $$PWD/ziplib/Source

# Statically link dependencies
LIBS += /usr/local/lib/libwthttp.a
LIBS += /usr/local/lib/libwt.a

#LIBS += /usr/lib/libGraphicsMagick.a
#LIBS += /usr/lib/x86_64-linux-gnu/libfreetype.a
#LIBS += /usr/lib/x86_64-linux-gnu/libtiff.a

!equals( BUILD_OS, "UBUNTU_1604" ) {
    LIBS += /usr/lib/x86_64-linux-gnu/liblcms.a
}

LIBS += $$ICU_PATH/libicui18n.a
LIBS += $$ICU_PATH/libicuuc.a

LIBS += /usr/lib/x86_64-linux-gnu/libglib-2.0.a
LIBS += /usr/lib/x86_64-linux-gnu/libgobject-2.0.a
#LIBS += /usr/lib/x86_64-linux-gnu/libpango-1.0.a
#LIBS += /usr/lib/x86_64-linux-gnu/libpangoft2-1.0.a
#LIBS += /usr/lib/x86_64-linux-gnu/libpng12.a
LIBS += $$ICU_PATH/libicudata.a
LIBS += /usr/lib/x86_64-linux-gnu/libgmodule-2.0.a
#LIBS += /usr/lib/x86_64-linux-gnu/libfontconfig.a
LIBS += /usr/lib/x86_64-linux-gnu/libltdl.a
LIBS += /usr/lib/x86_64-linux-gnu/libxml2.a

#LIBS += /usr/lib/x86_64-linux-gnu/libbz2.a
#LIBS += /usr/lib/x86_64-linux-gnu/libwmflite.a
#LIBS += /usr/lib/x86_64-linux-gnu/libjasper.a
LIBS += /usr/lib/x86_64-linux-gnu/libjpeg.a

LIBS += $$BOOST_PATH/libboost_thread.a
LIBS += $$BOOST_PATH/libboost_filesystem.a
LIBS += $$BOOST_PATH/libboost_signals.a
LIBS += $$BOOST_PATH/libboost_regex.a
LIBS += $$BOOST_PATH/libboost_random.a
LIBS += $$BOOST_PATH/libboost_date_time.a
LIBS += $$BOOST_PATH/libboost_program_options.a
LIBS += $$BOOST_PATH/libboost_system.a

LIBS += -lrt

LIBS += /usr/lib/x86_64-linux-gnu/libssl.a
LIBS += /usr/lib/x86_64-linux-gnu/libcrypto.a
LIBS += /usr/lib/x86_64-linux-gnu/libcrypt.a
LIBS += /usr/lib/x86_64-linux-gnu/libz.a
LIBS += /usr/lib/x86_64-linux-gnu/libm.a

equals( BUILD_OS, "UBUNTU_1604" ) {
#    LIBS += /usr/lib/gcc/x86_64-linux-gnu/5/libgomp.a
} else {
#    LIBS += /usr/lib/gcc/x86_64-linux-gnu/4.8/libgomp.a
}

LIBS += /usr/lib/x86_64-linux-gnu/libX11.a
LIBS += /usr/lib/x86_64-linux-gnu/libXext.a
LIBS += /usr/lib/x86_64-linux-gnu/libxcb.a
LIBS += /usr/lib/x86_64-linux-gnu/libXau.a
LIBS += /usr/lib/x86_64-linux-gnu/libXdmcp.a

equals( BUILD_OS, "UBUNTU_1604" ) {
    LIBS += -lpthread -ldl -lc -lm -static
    TEMPLATE = app
    CONFIG = console
} else {
    LIBS += -ldl
    TEMPLATE = app
}

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
    yw_configpage_ymgenerator.cpp \
    yw_configpage_modules.cpp \
    yw_configpage_update.cpp \
    yw_modulemanifest.cpp \
    yw_servermanifest.cpp

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
    yw_configpage_ymgenerator.h \
    yw_configpage_modules.h \
    yw_configpage_update.h \
    yw_modulemanifest.h \
    yw_servermanifest.h

target.path = /opt/yarra
INSTALLS += target

