QT       += core gui multimedia axcontainer charts network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

LIBS += -lPropsys -lwinmm

SOURCES += \
    abstractcapturethread.cpp \
    anycapture.cpp \
    audiocapture.cpp \
    main.cpp \
    mainwindow.cpp \
    netspeaker.cpp

HEADERS += \
    abstractcapturethread.h \
    anycapture.h \
    audiocapture.h \
    audiocaptured.h \
    captureglobal.h \
    mainwindow.h \
    netspeaker.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
