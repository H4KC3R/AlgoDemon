CONFIG += c++2a

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include

INCLUDEPATH += $$PWD/include/CameraSDK_include
DEPENDPATH += $$PWD/include/CameraSDK_include

HEADERS  += \
    $$PWD/include/autoexposurehandler.h \
    $$PWD/include/camenums.h \
    $$PWD/include/cameraqhyccd.h \
    $$PWD/include/camframe.h \
    $$PWD/include/camstruct.h \
    $$PWD/include/framepipeline.h \
    $$PWD/include/imageblurmetric.h \
    $$PWD/include/imageprocess.h \
    $$PWD/include/objectivecontroller.h\
    $$PWD/include/serialib.h

SOURCES += \
        $$PWD/src/camframe.cpp \
        $$PWD/src/autoexposurehandler.cpp\
        $$PWD/src/cameraqhyccd.cpp \
        $$PWD/src/framepipeline.cpp \
        $$PWD/src/imageblurmetric.cpp \
        $$PWD/src/imageprocess.cpp \
        $$PWD/src/objectivecontroller.cpp \
        $$PWD/src/serialib.cpp


LIBS += -L$$PWD/lib/win64 -lqhyccd
LIBS += -L$$PWD/lib/win64/opencv-lib -llibopencv_calib3d460     -llibopencv_core460
LIBS += -L$$PWD/lib/win64/opencv-lib -llibopencv_features2d460  -llibopencv_flann460
LIBS += -L$$PWD/lib/win64/opencv-lib -llibopencv_highgui460     -llibopencv_imgproc460  -llibopencv_xphoto460
