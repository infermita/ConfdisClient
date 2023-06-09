#-------------------------------------------------
#
# Project created by QtCreator 2016-11-04T10:34:30
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

VERSION = 1.3.5

TARGET = confdis
#target.path = /usr/bin
#INSTALLS += target 

#_db.files = db/dbwfp
#_db.path = /etc/condis

#_image.files = images/wfp.jpg
#_image.path = /etc/condis

#_service.files = service/condis.service 
#_service.path = /etc/systemd/system/

#INSTALLS += _db _image

#QMAKE_POST_LINK = $$quote(systemctl enable condis) 

TEMPLATE = app
CONFIG+=debug
CONFIG += link_pkgconfig
PKGCONFIG += libnfc
#DESTDIR=/usr/bin
#DESTDIR=$(CURDIR)/debian/project
LIBS += -lwiringPi
SOURCES += main.cpp\
        mainwindow.cpp \
    lib/socketserver.cpp \
    lib/nfcthread.cpp \
    lib/dao.cpp \
    lib/ledthread.cpp \
    lib/socketclient.cpp \
    lib/jsoncommand.cpp \
    lib/socketserveralive.cpp \
    lib/ntpdate.cpp

HEADERS  += mainwindow.h \
    lib/socketserver.h \
    lib/nfcthread.h \
    lib/dao.h \
    lib/ledthread.h \
    lib/socketclient.h \
    lib/jsoncommand.h \
    lib/socketserveralive.h \
    lib/ntpdate.h

FORMS    += mainwindow.ui
