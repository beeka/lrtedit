QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_testluaparser.cpp \
    ../luagenerator.cpp \
    ../luaparser.cpp \
    ../luatable.cpp

HEADERS += \
    ../luagenerator.h \
    ../luaparser.h \
    ../luatable.h

INCLUDEPATH += ..
