#-------------------------------------------------
#
# Project created by QtCreator 2013-04-09T14:41:52
#
#-------------------------------------------------

QT       += core gui
QT       +=network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TestMachine
TEMPLATE = app


SOURCES += main.cpp\
        testmachine.cpp \
    engine/lascanner.cpp \
    engine/laprinter.cpp \
    engine/lahw.cpp \
    engine/labcr.cpp \
    engine/common.cpp \
    printthread.cpp \
    socketthread.cpp \
    engine/hwinfoquery.cpp \
    choiceotherpro.cpp

HEADERS  += testmachine.h \
    engine/lascanner.h \
    engine/laprinter.h \
    engine/lahw.h \
    engine/labcr.h \
    engine/def.h \
    engine/common.h \
    printthread.h \
    socketthread.h \
    engine/hwinfoquery.h \
    TypeDef.h \
    engine/choiceotherpro.h \
    choiceotherpro.h

FORMS    += testmachine.ui \
    choiceotherpro.ui

#deployment
sources.files = TestMachine
target.path = /etc/LATECH/APP
sources.path = TestMachine
INSTALLS += target

RESOURCES += \
    image.qrc
