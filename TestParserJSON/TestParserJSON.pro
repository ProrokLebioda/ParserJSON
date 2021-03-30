QT += core testlib
QT += network
QT -= gui
TARGET = TestParserJSON
CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_testparserjson.cpp

INCLUDEPATH +=$$PWD/../ParserJSONLib

# Adds the QtQuickSampleLib.lib to the linker
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ParserJSONLib/release/ -lParserJSONLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ParserJSONLib/debug/ -lParserJSONLib
