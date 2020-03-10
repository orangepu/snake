SOURCES += \
    main.cpp \
    snake.cpp \
    connect.cpp

QT += widgets

QT +=core gui network

CONFIG += c++11

RESOURCES += \
    res.qrc

RC_ICONS += images/logo.ico

HEADERS += \
    snake.h \
    connect.h
