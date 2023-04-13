QT += testlib
QT += core gui widgets sql
CONFIG += qt warn_on depend_includepath testcase

TEMPLATE = app

SOURCES += \
    ../Interface/src/receivedata.cpp \
    ../Interface/src/sendcommands.cpp \
    ../Interface/src/threadcontroller.cpp \
    ../Interface/src/dbmanager.cpp \
    ../Interface/src/densification.cpp \
    ../Interface/src/experiment.cpp \
    ../Interface/src/field.cpp \
    ../Interface/src/shear.cpp \
    ../Interface/src/table.cpp \
    src/machineclient.cpp \
    src/machineserver.cpp \
    src/tst_sockettests.cpp



HEADERS += \
    ../Interface/inc/receivedata.h \
    ../Interface/inc/sendcommands.h \
    ../Interface/inc/socket_local.h \
    ../Interface/inc/threadcontroller.h \
    ../Interface/inc/dbmanager.h \
    ../Interface/inc/densification.h \
    ../Interface/inc/experiment.h \
    ../Interface/inc/field.h \
    ../Interface/inc/shear.h \
    ../Interface/inc/table.h \
    inc/machineclient.h \
    inc/machineserver.h


