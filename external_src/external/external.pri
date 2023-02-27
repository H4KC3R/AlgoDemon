#INCLUDEPATH += $$PWD
#DEPENDPATH += $$PWD

CONFIG += c++2a

ADS_OUT_ROOT = $$PWD/Qt-Advanced-Docking-System-master
LIBS += -L$${ADS_OUT_ROOT}/lib
DESTDIR = $${ADS_OUT_ROOT}/lib
include($${ADS_OUT_ROOT}/ads.pri)
INCLUDEPATH += $${ADS_OUT_ROOT}/src
DEPENDPATH += $${ADS_OUT_ROOT}/src

#message($${ADS_OUT_ROOT}/src)






