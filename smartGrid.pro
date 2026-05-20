QT += core gui widgets

CONFIG += c++17

TARGET = SmartGridUI
TEMPLATE = app

INCLUDEPATH += include

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/Appliance.cpp \
    src/GridController.cpp \
    src/InductiveLoad.cpp \
    src/ResistiveLoad.cpp \
    src/BillingEngine.cpp \
    src/Auditor.cpp

HEADERS += \
    include/mainwindow.h \
    include/Appliance.h \
    include/ApplianceState.h \
    include/GridController.h \
    include/InductiveLoad.h \
    include/ResistiveLoad.h \
    include/BillingEngine.h \
    include/Auditor.h