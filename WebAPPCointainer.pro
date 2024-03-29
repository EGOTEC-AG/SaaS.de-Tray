#-------------------------------------------------
#
# Project created by QtCreator 2013-11-22T14:47:44
#
#-------------------------------------------------

QT += core gui webengine webenginewidgets network sql svg xml

TARGET = Timerecording
TEMPLATE = app

DEFINES += "WINVER=0x0500"

CONFIG += static

# WINDOWS CONFIGS
win32 {
    CONFIG += openssl-linked

    INCLUDEPATH += C:/OpenSSL-Win64/include
    LIBS += -LC:/OpenSSL-Win64/lib -llibcrypto -llibssl

    SOURCES +=  main.cpp
    QMAKE_LIBS += user32.lib

    win32:RC_ICONS += icon/tray.ico
}
 else {
    SOURCES +=  main.cpp
}

# MAC CONFIGS
macx {
    QMAKE_INFO_PLIST = macosx/Info.plist
    ICON = icon/favicon.icns
    QMAKE_MAC_SDK = macosx12.0
}
RESOURCES += images.qrc

SOURCES += mainwindow.cpp \
    bridgecontroller.cpp \
    filedownloader.cpp \
    json/JSON.cpp \
    json/JSONValue.cpp \
    webview.cpp \
    helper/logger.cpp

HEADERS  += mainwindow.h \
    bridgecontroller.h \
    filedownloader.h \
    json/JSON.h \
    json/JSONValue.h \
    webview.h \
    helper/logger.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    icon/tray.png \
    icon/clock_stop.png \
    icon/favicon.icns \
    macosx/Info.plist \
    icon/window_size.png \
    icon/exit.png

DISTFILES +=
