#-------------------------------------------------
#
# Project created by QtCreator 2018-10-08T13:06:07
#
#-------------------------------------------------

QT += core network gui dbus
CONFIG += c++17
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = teszt1
DEFINES += QT_TARGET='\\"$$TARGET\\"'
message( "TARGET = "$$TARGET )

TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
unix:HOME = $$system(echo $HOME)
win32:HOME = $$system(echo %userprofile%)

CONFIG(debug, debug|release){
BUILD = debug
} else {
BUILD = release
}

COMMON_LIBS = commonlib

equals(BUILD,debug) {
    #message( "build is _ debug" )
    COMMON_LIBS = $$COMMON_LIBS"_debug"
}

equals(BUILD,release){
    COMMON_LIBS = $$COMMON_LIBS"_release"
}

equals(QT_ARCH, x86_64){
    COMMON_LIBS = $$COMMON_LIBS"_64"
}

equals(QT_ARCH, x86){
    COMMON_LIBS = $$COMMON_LIBS"_32"
}

equals(QT_ARCH, arm){
#    COMMON_LIBS = $$COMMON_LIBS"_arm"
    MV33 = $$find(QMAKE_CFLAGS, -march.*)
    MV34 = $$split(MV33, =)
    MARCH = $$member(MV34, 1)
    COMMON_LIBS = $$COMMON_LIBS"_"$$MARCH
}

message( "architecture = "$$QT_ARCH )
message( "commonlibs folder = "$$COMMON_LIBS )

#unix:HOME = $$system(echo $HOME)
#win32:HOME = $$system(echo %userprofile%)

# INSTALLDIR = $$COMMON_LIBS
COMMON_LIBS_FULLPATH = $$shell_path($$HOME/$$COMMON_LIBS)

message( "COMMON_LIBS_FULLPATH = "$$COMMON_LIBS_FULLPATH)

#unix:!macx: LIBS += -L/home/zoli/build-common-Desktop_Qt_5_12_2_GCC_64bit2-Debug/stringhelper/ -lstringhelper
unix:!macx:
{
LIBS += -L$$COMMON_LIBS_FULLPATH -llogger
LIBS += -L$$COMMON_LIBS_FULLPATH -lsignalhelper
LIBS += -L$$COMMON_LIBS_FULLPATH -lCommandLineParserHelper
LIBS += -L$$COMMON_LIBS_FULLPATH -lcoreappworker
LIBS += -L$$COMMON_LIBS_FULLPATH -lstringhelper
LIBS += -L$$COMMON_LIBS_FULLPATH -ltextfilehelper
LIBS += -L$$COMMON_LIBS_FULLPATH -lfilehelper
LIBS += -L$$COMMON_LIBS_FULLPATH -lmacrofactory
LIBS += -L$$COMMON_LIBS_FULLPATH -lshortguid
#LIBS += -L$$COMMON_LIBS_FULLPATH/ -linihelper
#LIBS += -L$$COMMON_LIBS_FULLPATH/ -lsettingshelper
#LIBS += -L$$COMMON_LIBS_FULLPATH/ -lxmlhelper
}

# unix:QMAKE_RPATHDIR += /usr/mylib
# QMAKE_LFLAGS += -Wl,-rpath, "$$system_path($$HOME/$$INSTALLDIR)"
#QMAKE_LFLAGS += -Wl,-rpath,"$$HOME/$$COMMON_LIBS_FULLPATH"
#QMAKE_LFLAGS += -Wl,-rpath,"/$$HOME/$$INSTALLDIR/macrofactory"
#QMAKE_LFLAGS += -Wl,-rpath,"/home/zoli/build-common-Desktop_Qt_5_9_0_GCC_64bit-Debug/stringhelper"
#QMAKE_LFLAGS += -Wl,-rpath,"/home/zoli/build-common-Desktop_Qt_5_9_0_GCC_64bit-Debug/macrofactory"
#QMAKE_LFLAGS += -Wl,-rpath,"/home/zoli/build-common-Desktop_Qt_5_12_2_GCC_64bit2-Debug/stringhelper"
#QMAKE_LFLAGS += -Wl,-rpath,"/home/zoli/build-common-Desktop_Qt_5_12_2_GCC_64bit2-Debug/macrofactory"
#INCLUDEPATH += $$HOME/common

INCLUDEPATH += $$HOME/common
DEPENDPATH += $$HOME/common

SOURCES += \
    actionresult.cpp \
    aliveworker.cpp \
    buildnumber.cpp \
    cmdworkerthread.cpp \
    command.cpp \
    doworkcmd.cpp \
    httpthreadedserver.cpp \
    instance.cpp \
        main.cpp \
        mainwindow.cpp \
    mounthelper.cpp \
    pressurehelperevent.cpp \
    processhelper.cpp \
    recordhelper.cpp \
    recordworker.cpp \
    request.cpp \
    response.cpp \
    settings.cpp \
    shutdowner.cpp \
    testhelper.cpp \
    dowork.cpp \
    userhelper.cpp \
    ztcpsocket.cpp \
    zthreadedsockethandler.cpp \
    zactionhelper.cpp \
    zactiontask.cpp \
    zlogicexception.cpp \
    zstringhelper.cpp \
    pressurehelper.cpp \
    zsockethandler.cpp \
    updater.cpp \
    restarter.cpp \
    zudpsocket.cpp

HEADERS += \
    actionresult.h \
    actionworker.h \
    aliveworker.h \
    buildnumber.h \
    cmdworkerthread.h \
    command.h \
    doworkcmd.h \
    doworker.h \
    httpthreadedserver.h \
    instance.h \
        mainwindow.h \
    mounthelper.h \
    pressurehelperevent.h \
    processhelper.h \
    recordhelper.h \
    recordworker.h \
    request.h \
    response.h \
    settings.h \
    shutdowner.h \
    testhelper.h \
    dowork.h \
    userhelper.h \
    ztcpsocket.h \
    zthreadedsockethandler.h \
    zactiontask.h \
    zactionhelper.h \
    globals.h \
    zlogicexception.h \
    zstringhelper.h \
    pressurehelper.h \
    zsockethandler.h \
    updater.h \
    restarter.h \
    zudpsocket.h

FORMS += \
        mainwindow.ui


contains(QMAKESPEC,.*linux-rasp-pi\d*-.*){
    message(rpi)
    CONFIG += rpi
    DEFINES += RPI
}

# Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
#!isEmpty(target.path): INSTALLS += targe

# if defined(Q_PROCESSOR_ARM) DEFINES += RPI

# Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android:{
#contains(QMAKE_CXX, arm.*):{
#        DEFINES += RPI
unix:rpi:{
message(LIBS added for raspberry_pi)
#wiringPI a GPIO kezeléséhez
LIBS += -lwiringPi
#LIBS += -lnfc -lpthread
#LIBS += -lusb-1.0
}


## definíció a feltételes fordításhoz
#    # DEFINES += RPI=true
#}
#!isEmpty(target.path): INSTALLS += target
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /home/pi/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
