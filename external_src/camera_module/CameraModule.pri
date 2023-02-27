CONFIG += c++2a

HEADERS  += \
    $$PWD/include/autoexposurehandler.h \
    $$PWD/include/camenums.h \
    $$PWD/include/cameraqhyccd.h \
    $$PWD/include/camstruct.h \
    $$PWD/include/imageblurmetric.h \
    $$PWD/include/imageprocess.h \
    $$PWD/include/objectivecontroller.h\
    $$PWD/include/serialib.h

SOURCES += \
        $$PWD/src/autoexposurehandler.cpp\
        $$PWD/src/cameraqhyccd.cpp \
        $$PWD/src/imageblurmetric.cpp \
        $$PWD/src/imageprocess.cpp \
        $$PWD/src/objectivecontroller.cpp \
        $$PWD/src/serialib.cpp


INCLUDEPATH += -I$$PWD/include/CameraSDK_include
INCLUDEPATH += -I$$PWD/include/opencv2
LIBS += -L$$PWD/lib

