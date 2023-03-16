QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++2a
QT += concurrent
QT += serialport

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    appprocessor.cpp \
    camerathread.cpp \
    main.cpp \
    mainwindow.cpp \
    mattoqimage.cpp \
    objectivethread.cpp \
    processingthread.cpp

HEADERS += \
    appprocessor.h \
    camerathread.h \
    mainwindow.h \
    mattoqimage.h \
    objectivethread.h \
    processingthread.h \
    structures.h

FORMS += \
    mainwindow.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

include(../../external_src/AlgoDemon.pri)
include(../../external_src/camera_module/CameraModule.pri)

RESOURCES += \
    resources.qrc
