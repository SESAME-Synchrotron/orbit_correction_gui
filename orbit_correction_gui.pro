QT       += core gui xml uitools

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
TARGET = $$(HOME)/bin/orbit-qt

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    circularbuffer.cpp \
    client.cpp \
    expert.cpp \
    horizontalcorrectors.cpp \
    logs.cpp \
    macros.cpp \
    main.cpp \
    mainwindow.cpp \
    plots.cpp \
    verticalcorrectors.cpp

HEADERS += \
    circularbuffer.h \
    client.h \
    expert.h \
    horizontalcorrectors.h \
    logs.h \
    macros.h \
    mainwindow.h \
    plots.h \
    verticalcorrectors.h

FORMS += \
    expert.ui \
    horizontalcorrectors.ui \
    logs.ui \
    mainwindow.ui \
    plots.ui \
    verticalcorrectors.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

unix:!macx: LIBS += -L$$(QE_TARGET_DIR)/lib/linux-x86_64/ -lQEFramework
unix:!macx: LIBS += -L$$(QE_TARGET_DIR)/lib/linux-x86_64/designer -lQEPlugin

INCLUDEPATH += $$(QE_TARGET_DIR)/include
DEPENDPATH  += $$(QE_TARGET_DIR)/include

unix:!macx: LIBS += -L$$(QWT_ROOT)/lib/ -lqwt

INCLUDEPATH += $$(QWT_ROOT)/include
DEPENDPATH  += $$(QWT_ROOT)/include

unix:!macx: LIBS += -L$$(EPICS_BASE)/lib/linux-x86_64/ -lca
unix:!macx: LIBS += -L$$(EPICS_BASE)/lib/linux-x86_64/ -lCom

INCLUDEPATH += $$(EPICS_BASE)/include
DEPENDPATH += $$(EPICS_BASE)/include

INCLUDEPATH += $$(EPICS_BASE)/include/os/Linux
DEPENDPATH += $$(EPICS_BASE)/include/os/Linux

INCLUDEPATH += $$(EPICS_BASE)/include/compiler/gcc/
