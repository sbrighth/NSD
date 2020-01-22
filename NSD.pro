QT       += core gui bluetooth

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    adddevicegroup.cpp \
    ble/characteristicinfo.cpp \
    ble/device.cpp \
    ble/deviceinfo.cpp \
    ble/serviceinfo.cpp \
    bleserviceviewer.cpp \
    devicemanagement.cpp \
    finddevice.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    adddevicegroup.h \
    ble/characteristicinfo.h \
    ble/device.h \
    ble/deviceinfo.h \
    ble/serviceinfo.h \
    bleserviceviewer.h \
    devicemanagement.h \
    finddevice.h \
    mainwindow.h

FORMS += \
    adddevicegroup.ui \
    devicemanagement.ui \
    finddevice.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc

DISTFILES +=