#-------------------------------------------------
#
# Project created by QtCreator 2017-01-30T18:50:19
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = QMixpanel
TEMPLATE = lib

DEFINES += QMIXPANEL_LIBRARY

SOURCES += qmixpanel.cpp

HEADERS += qmixpanel.h\
        qmixpanel_global.h \
    util.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
