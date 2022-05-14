QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    annotationlist.cpp \
    annotationlistmodel.cpp \
    main.cpp \
    mainwindow.cpp \
    vwidget.cpp\
    arisdata.cpp\
    arisframe.cpp\
    didsondata.cpp\
    didsonframe.cpp\
    sonardata.cpp\
    imageprocessing.cpp\
    pinglist.cpp\
    coloraction.cpp\
    ewidget.cpp

HEADERS += \
    annotationlist.h \
    annotationlistmodel.h \
    mainwindow.h \
    vwidget.h\
    arisdata.h\
    arisframe.h\
    didsondata.h\
    didsonframe.h\
    sonardata.h\
    imageprocessing.h\
    pinglist.h\
    coloraction.h\
    ewidget.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

macx{
    ICON = Fish_Annotation_Icon.icns
}

win32-g++{
    RC_FILE = sonarannotator.rc.rc
}
