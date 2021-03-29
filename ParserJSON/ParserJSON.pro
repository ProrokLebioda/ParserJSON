QT += core
QT += network
QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp

INCLUDEPATH +=$$PWD/../ParserJSONLib

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Adds the QtQuickSampleLib.lib to the linker
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ParserJSONLib/release/ -lParserJSONLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ParserJSONLib/debug/ -lParserJSONLib
