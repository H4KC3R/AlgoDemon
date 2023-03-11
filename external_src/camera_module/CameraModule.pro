QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp\
        src/autoexposurehandler.cpp\
        src/cameraqhyccd.cpp \
        src/imageblurmetric.cpp \
        src/imageprocess.cpp \
        src/objectivecontroller.cpp \
        src/imagepipeline.cpp\
        src/camframe.cpp\
        src/serialib.cpp

HEADERS  += \
    include/autoexposurehandler.h \
    include/camenums.h \
    include/cameraqhyccd.h \
    include/camstruct.h \
    include/camframe.h\
    include/imageblurmetric.h \
    include/imageprocess.h \
    include/objectivecontroller.h\
    include/imagepipeline.h\
    include/serialib.h

INCLUDEPATH += include
DEPENDPATH += include

unix: {
    message("Unix build. To DO")
}

win32: {
    message($$PWD)
    contains(QT_ARCH, i386) {
        message("x86 build")
    }
    else {
        message("x86_64 build")
        INCLUDEPATH += -I$$PWD/include/CameraSDK_include
        INCLUDEPATH += -I$$PWD/include/opencv2

        LIBS += -L$$PWD/lib/win64 -lqhyccd
        LIBS += -L$$PWD/lib/win64/opencv-lib -llibopencv_calib3d460     -llibopencv_core460
        LIBS += -L$$PWD/lib/win64/opencv-lib -llibopencv_features2d460  -llibopencv_flann460
        LIBS += -L$$PWD/lib/win64/opencv-lib -llibopencv_highgui460     -llibopencv_imgproc460  -llibopencv_xphoto460
    }
}

