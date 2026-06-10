QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    csv_reader.c \
    grid.c \
    logic.c \
    main.cpp \
    matrix.c \
    mainwindow.cpp \
    scene.c \
    surface.c

HEADERS += \
    appcontext.h \
    csv_reader.h \
    geometry.h \
    grid.h \
    logic.h \
    matrix.h \
    scene.h \
    status.h \
    mainwindow.h \
    surface.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resourses.qrc
