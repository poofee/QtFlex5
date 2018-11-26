#-------------------------------------------------
#
# Project created by QtCreator 2015-08-18T14:31:53
#
#-------------------------------------------------

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets gui-private widgets-private

TARGET = Qt5Flex

CONFIG(debug, debug|release) {
    TARGET = $$member(TARGET, 0)d
    DESTDIR = $$_PRO_FILE_PWD_/../bin
} else {
    DESTDIR = $$_PRO_FILE_PWD_/../bin
}


TEMPLATE = lib

QMAKE_CXXFLAGS += -fpermissive#-std=c++11 -stdlib=libc++

DEFINES += QT_FLEX_BUILD

#INCLUDEPATH += C:/Qt/Qt5.4.2/5.4/msvc2013_64_opengl/include/QtCore/5.4.2/QtCore
#INCLUDEPATH += C:/Qt/Qt5.4.2/5.4/msvc2013_64_opengl/include/QtGui/5.4.2/QtGui
#INCLUDEPATH += C:/Qt/Qt5.4.2/5.4/msvc2013_64_opengl/include/QtWidgets/$$QT_VERSION/QtWidgets
#INCLUDEPATH += C:/Qt/Qt5.4.2/5.4/msvc2013_64_opengl/include/QtWidgets
#INCLUDEPATH += C:/Qt/Qt5.4.2/5.4/msvc2013_64_opengl/include/QtGui/5.4.2/QtGui/qpa

SOURCES += \
    QtDockGuider.cpp \
    QtDockSide.cpp \
    QtDockSite.cpp \
    QtDockWidget.cpp \
    QtFlexHelper.cpp \
    QtFlexManager.cpp \
    QtFlexStyle.cpp \
    QtFlexWidget.cpp

HEADERS += \
    QtDockGuider.h \
    QtDockSide.h \
    QtDockSite.h \
    QtDockWidget.h \
    QtFlexHelper.h \
    QtFlexManager.h \
    QtFlexStyle.h \
    QtFlexWidget.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

RESOURCES += \
    QtFlex.qrc
