# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Add-in.
# ------------------------------------------------------

QT += core sql widgets gui
TARGET = retek2
TEMPLATE = app

HEADERS += Beallitasok.h \
    retek2.h \
    ztablerow.h \
    ztable.h \
    ztable.h \
#    zdatabase.h \
    zstringmaphelper.h \
    zfilenamehelper.h \
    zlog.h \
    globals.h \
    zsql.h \
    zstringhelper.h
SOURCES += Beallitasok.cpp \
    main.cpp \
    retek2.cpp \
    ztable.cpp \
    ztablerow.cpp \
#    zdatabase.cpp \
    zstringmaphelper.cpp \
    zfilenamehelper.cpp \
    zlog.cpp \
    globals.cpp \
    zsql.cpp \
    zstringhelper.cpp
FORMS += retek2.ui
RESOURCES += retek2.qrc

CONFIG += debug
DEFINES += QT_DLL QT_SQL_LIB QT_WIDGETS_LIB

CONFIG += c++14
CONFIG += warn_on

# LIBS += -L$$PWD/../../mysql/lib/ -lmysqlcppconn

#INCLUDEPATH += $$PWD/../../mysql/include
#DEPENDPATH += $$PWD/../../mysql/include

#QMAKE_CXXFLAGS += -std=c++14
#QMAKE_CXXFLAGS += -Wall

#INCLUDEPATH += ./GeneratedFiles \
#    . \
#    ./GeneratedFiles/Debug
#DEPENDPATH += .
#MOC_DIR += ./GeneratedFiles/debug
#OBJECTS_DIR += debug
#UI_DIR += ./GeneratedFiles
#RCC_DIR += ./GeneratedFiles

# win32:RC_FILE = retek2.rc

# win32:CONFIG(release, debug|release): LIBS += -L$$PWD/'C:/Program Files/MySQL/MySQL Connector C++ 1.1.9/lib/opt/' -lmysqlcppconn.dll
# else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/'C:/Program Files/MySQL/MySQL Connector C++ 1.1.9/lib/opt/' -lmysqlcppconn
# else:unix:!macx: LIBS += -L$$PWD/'C:/Program Files/MySQL/MySQL Connector C++ 1.1.9/lib/opt/' -lmysqlcppconn

#INCLUDEPATH += $$PWD/'C:/Program Files/MySQL/MySQL Connector C++ 1.1.9/lib/opt'
#DEPENDPATH += $$PWD/'C:/Program Files/MySQL/MySQL Connector C++ 1.1.9/lib/opt'

# INCLUDEPATH += C:\Program Files\MySQL\MySQL Connector C++ 1.1.9\lib\opt
# DEPENDPATH += C:\Program Files\MySQL\MySQL Connector C++ 1.1.9\lib\opt

