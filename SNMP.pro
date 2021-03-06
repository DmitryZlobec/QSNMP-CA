include(D:\_Development\qt-solutions-master\qtservice\src\qtservice.pri)

QT -= gui
QT += network

CONFIG += c++11 console
CONFIG -= app_bundle
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
        main.cpp \
    applicationclass.cpp \
    snmpserver.cpp \
    qoid.cpp \
    snmpgetrequset.cpp \
    snmprequest.cpp \
    param.cpp \
    appconfig.cpp \
    snmpservice.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    applicationclass.h \
    snmpserver.h \
    qoid.h \
    snmpgetrequset.h \
    snmprequest.h \
    param.h \
    appconfig.h \
    snmpservice.h



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../libmongocxx/lib/ -llibbsoncxx.dll
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../libmongocxx/lib/ -llibbsoncxx.dll
else:unix: LIBS += -L$$PWD/../../libmongocxx/lib/ -llibbsoncxx.dll

INCLUDEPATH += $$PWD/../../libmongocxx/include/bsoncxx/v_noabi
DEPENDPATH += $$PWD/../../libmongocxx/include/bsoncxx/v_noabi

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../libmongocxx/lib/ -llibmongocxx.dll
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../libmongocxx/lib/ -llibmongocxx.dll
else:unix: LIBS += -L$$PWD/../../libmongocxx/lib/ -llibmongocxx.dll

INCLUDEPATH += $$PWD/../../libmongocxx/include/mongocxx/v_noabi
DEPENDPATH += $$PWD/../../libmongocxx/include/mongocxx/v_noabi
