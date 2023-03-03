INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

CONFIG += c++2a


include(external/external.pri)
include(widgets/widgets.pri)

#CONFIG += conan_basic_setup
#include(../conan/conanbuildinfo.pri)

HEADERS +=

SOURCES +=


