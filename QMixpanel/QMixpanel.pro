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


HEADERS += qmixpanel.h\
        qmixpanel_global.h \
    util.h \
    qmixpanelprofile.h \
    qmixpanelevent.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

SOURCES += \
    qmixpanel.cpp
