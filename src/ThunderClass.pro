QT       += core gui

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
    Connection.cpp \
    Image.cpp \
    Message.cpp \
    Pixel.cpp \
    Server.cpp \
    activeinfowindow.cpp \
    admin.cpp \
    adminprocess.cpp \
    adminwindow.cpp \
    answerwindow.cpp \
    audio.cpp \
    audioframe.cpp \
    client.cpp \
    log.cpp \
    loginprocess.cpp \
    main.cpp \
    loginwindow.cpp \
    questionwindow.cpp \
    student.cpp \
    studentprocess.cpp \
    studentwindow.cpp \
    stutimeinfo.cpp \
    teacher.cpp \
    teacherprocess.cpp \
    teacherwindow.cpp \
    user.cpp

HEADERS += \
    Connection.h \
    Constant.h \
    Image.h \
    Message.h \
    MessageType.h \
    Pixel.h \
    Server.h \
    activeinfowindow.h \
    admin.h \
    adminprocess.h \
    adminwindow.h \
    answerwindow.h \
    audio.h \
    audioframe.h \
    client.h \
    log.h \
    loginprocess.h \
    loginwindow.h \
    questionwindow.h \
    student.h \
    studentprocess.h \
    studentwindow.h \
    stutimeinfo.h \
    teacher.h \
    teacherprocess.h \
    teacherwindow.h \
    user.h

FORMS += \
    activeinfowindow.ui \
    adminwindow.ui \
    answerwindow.ui \
    loginwindow.ui \
    questionwindow.ui \
    studentwindow.ui \
    teacherwindow.ui

LIBS += -lws2_32 \
    -lgdi32 \
    -lgdiplus \
    -lwinmm \
    -lole32

DEFINES += QT_MESSAGELOGCONTEXT

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resource.qrc

RC_FILE += myico.rc
