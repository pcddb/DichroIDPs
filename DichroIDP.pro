#-------------------------------------------------
#
# Project created by QtCreator 2018-12-12T17:39:27
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = DichroIDP
TEMPLATE = app
#RC_FILE = myApp.rc

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

#CONFIG += no_keywords

SOURCES += \
    aboutdlg.cpp \
        main.cpp \
        mainwindow.cpp \
    ../../Qt/QCustomPlot/qcustomplot/qcustomplot.cpp \
    interface.cpp \
    read_file.cpp \
    selcon.cpp \
    rmsd.cpp \
    sortrows.cpp\
    alglib/cpp/src/statistics.cpp \
    alglib/cpp/src/ap.cpp \
    alglib/cpp/src/alglibinternal.cpp \
    alglib/cpp/src/alglibmisc.cpp \
    alglib/cpp/src/dataanalysis.cpp \
    alglib/cpp/src/specialfunctions.cpp \
    alglib/cpp/src/solvers.cpp \
    alglib/cpp/src/optimization.cpp \
    alglib/cpp/src/linalg.cpp \
    alglib/cpp/src/interpolation.cpp \
    alglib/cpp/src/integration.cpp \
    alglib/cpp/src/fasttransforms.cpp \
    alglib/cpp/src/diffequations.cpp \
    global.cpp \
    dialogdatset.cpp \
    borderdelegate.cpp \
    tableitemdelegate.cpp


HEADERS += \
    aboutdlg.h \
        mainwindow.h \
    ../../Qt/QCustomPlot/qcustomplot/qcustomplot.h \
    read_file.h \
    selcon.h \
    interface.h \
    rmsd.h \
    sortrows.h \
    global.h \
    dialogdatset.h \
    borderdelegate.h \
    tableitemdelegate.h


FORMS += \
    aboutdlg.ui \
        mainwindow.ui \
    interface.ui \
    dialogdatset.ui \
   # dialogeditdat.ui

INCLUDEPATH += ../../../../Qt/QCustomPlot/qcustomplot/
INCLUDEPATH += alglib/cpp/src/
#LIBS += alglib/cpp/src/
#INCLUDEPATH +=../../../../
#INCLUDEPATH += ../
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#DISTFILES +=

RC_ICONS = DichroIDP.ico
INCLUDEPATH += /doc/
RESOURCES += \
    image.qrc
 #   logo2.qrc
#    DISTFILES += \   #     DichroApp.rc

DISTFILES +=

