QT += core testlib
QT -= gui
TARGET = TestParserJSON
CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_testparserjson.cpp
