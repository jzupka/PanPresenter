TARGET = Presenter

HEADERS       = glwidget.h \
    glimage.h \
    tools.h \
    cacheimage.h \
    animatorbase.h \
    paintCore.h \
    animators.h \
    loader.h \
    thumbnail.h \
    loaderimage.h \
    glimagevideo.h \
    glanimatedimage.h \
    bigimage.h \
    presenter.h
SOURCES       = glwidget.cpp \
                main.cpp \
    glimage.cpp \
    tools.cpp \
    cacheimage.cpp \
    animatorbase.cpp \
    animators.cpp \
    loader.cpp \
    thumbnail.cpp \
    loaderimage.cpp \
    glimagevideo.cpp \
    glanimatedimage.cpp \
    bigimage.cpp \
    presenter.cpp
RESOURCES     = Presenter.qrc
QT           += opengl multimedia multimediawidgets

# install
target.path = /usr/sbin
INSTALLS += target

unix {
    launcher.files = Presenter.desktop
    launcher.path = /usr/share/applications
    icons.files = icons/Presenter.png
    icons.path = /usr/share/pixmaps
    INSTALLS += launcher icons
}

unix:!macx:!symbian: LIBS += -lX11 -lgomp #-ltcmalloc
QMAKE_CXXFLAGS_RELEASE = -O3 -march=native -mtune=native -fopenmp -m64 -std=c++11
QMAKE_CXXFLAGS_DEBUG = -g3 -fopenmp -std=c++11
