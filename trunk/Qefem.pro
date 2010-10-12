# -------------------------------------------------
# Project created by QtCreator 2009-11-16T10:30:54
# -------------------------------------------------
TARGET = qefem
#DESTDIR = ./debian/qefem/usr/bin
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    fmpanel.cpp \
    controlpanel.cpp \
    copytask.cpp \
    deletetask.cpp \
    fmlistview.cpp \
    bookmarklistview.cpp \
    historylistview.cpp \
    global.cpp \
    findtask.cpp
HEADERS += mainwindow.h \
    fmpanel.h \
    controlpanel.h \
    copytask.h \
    deletetask.h \
    fmlistview.h \
    bookmarklistview.h \
    historylistview.h \
    global.h \
    findtask.h \
    defines.h
RESOURCES += qefem.qrc
win32:RC_FILE = qefem.rc
OTHER_FILES += readme.txt \
    bugstofix.txt
