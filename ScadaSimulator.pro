QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dialogs/aspcontroldialog.cpp \
    dialogs/dampercontroldialog.cpp \
    dialogs/eventlogdialog.cpp \
    equipment/aspiration.cpp \
    equipment/conveyor.cpp \
    equipment/damper.cpp \
    equipment/motor.cpp \
    equipment/plow.cpp \
    events/eventlogger.cpp \
    main.cpp \
    mainwindow.cpp \
    dialogs/motorcontroldialog.cpp \
    dialogs/plowcontroldialog.cpp \
    widgets/processwidget.cpp \
    widgets/toggleswitch.cpp

HEADERS += \
    dialogs/aspcontroldialog.h \
    dialogs/dampercontroldialog.h \
    dialogs/eventlogdialog.h \
    equipment/aspiration.h \
    equipment/conveyor.h \
    equipment/damper.h \
    equipment/motor.h \
    equipment/plow.h \
    equipment/processstates.h \
    events/eventlogger.h \
    mainwindow.h \
    dialogs/motorcontroldialog.h \
    dialogs/plowcontroldialog.h \
    widgets/processwidget.h \
    widgets/toggleswitch.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    ScadaSimulator_ru_KZ.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
