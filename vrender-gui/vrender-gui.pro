#
# Copyright (C) 2012 Evgeny Panov
# This file is part of vrender-gui.
#

QT += core gui opengl

TARGET = vrender-gui
TEMPLATE = app

INCLUDEPATH += include ../libvrender/include/
LIBS += -L../libvrender-build/ -lvrender

win32: LIBS += -lopengl32 -static -lgomp -lpthread
unix:  LIBS += -lGL -lgomp -Bstatic

PRE_TARGETDEPS += ../libvrender-build/libvrender.a

SOURCES += src/main.cpp \
		src/mainwindow.cpp \
		src/glwindow.cpp \
		src/helpdialog.cpp

HEADERS += include/mainwindow.h \
		include/glwindow.h \
		include/helpdialog.h

FORMS += ui/mainwindow.ui \
		ui/helpdialog.ui

RESOURCES += res/app.qrc

win32: RC_FILE = res/vrender.rc
