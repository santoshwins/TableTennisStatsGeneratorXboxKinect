# -------------------------------------------------
# Project created by QtCreator 2009-11-05T22:11:46
# -------------------------------------------------
QT += opengl
QT += core \
                gui

TARGET=KinectSportStatsGen

DESTDIR=./
OBJECTS_DIR=obj
# this is where we want to put the intermediate build files ( .o)
OBJECTS_DIR=./obj/
MOC_DIR=./moc/
SOURCES += src/main.cpp \
                                         src/MainWindow.cpp \
                                         src/ScreenQuad.cpp \
    src/BallTrackingVisualization.cpp \
    src/DepthDebugVisualization.cpp \
    src/KinectInterface.cpp \
    src/RGBboundsVisualization.cpp \
    src/StatsVisualization.cpp \
    src/QuadCurveFitUtility.cpp \
    src/BallTrackingUtility.cpp \
    src/PlayerData.cpp \
    src/BallPointsProcessing.cpp \
    src/TwoDStatsGeneration.cpp \
    src/ThreeDStatsGeneration.cpp \
    src/ExtendedQMdiSubWindow.cpp

HEADERS += include/MainWindow.h \
                                         include/ScreenQuad.h\
    include/BallTrackingVisualization.h \
    include/DepthDebugVisualization.h \
    include/KinectInterface.h \
    include/RGBboundsVisualization.h \
    include/StatsVisualization.h \
    include/QuadCurveFitUtility.h \
    include/BallTrackingUtility.h \
    include/PlayerData.h \
    include/BallPointsProcessing.h \
    include/TwoDStatsGeneration.h \
    include/ThreeDStatsGeneration.h \
    include/ExtendedQMdiSubWindow.h

OTHER_FILES+= shaders/TextureVert.glsl \
                                                        shaders/TextureFrag.glsl \
                                                        shaders/LumFrag.glsl \
    shaders/Colour.fs \
    shaders/Colour.vs \
    shaders/PerFragASDFrag.glsl \
    shaders/PerFragASDVert.glsl

DEFINES+=GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
INCLUDEPATH+=./include \
/usr/local/include/libfreenect \
$(HOME)/opencvbuild/include \
$(HOME)/gslbuild/include \
/usr/local/include

LIBS +=  -L/$(HOME)/opencvbuild/lib -lopencv_core \
         -lopencv_imgproc -lopencv_calib3d -lopencv_video -lopencv_features2d \
         -lopencv_ml -lopencv_highgui -lopencv_objdetect -lopencv_contrib -lopencv_legacy \
         -lopencv_nonfree -lopencv_flann -lopencv_photo -lopencv_gpu

#LIBS += -L/$(HOME)/gslbuild/lib -lgsl -lgslcblas

CONFIG += console
CONFIG -= app_bundle
DEFINES+=USING_QT_CREATOR

QMAKE_CXXFLAGS+= -msse -msse2 -msse3
macx:QMAKE_CXXFLAGS+= -arch x86_64
macx:INCLUDEPATH+=/usr/local/boost/
linux-g++:QMAKE_CXXFLAGS +=  -march=native
linux-g++-64:QMAKE_CXXFLAGS +=  -march=native

# define the _DEBUG flag for the graphics lib
DEFINES +=NGL_DEBUG

LIBS += -L/usr/local/lib

#this is for my laptop
LIBS += -L/usr/lib64

# add the ngl lib
LIBS +=  -L/$(HOME)/NGL/lib -l NGL
LIBS += -L/usr/local/lib64/ -lfreenect -lfreenect_sync
# now if we are under unix and not on a Mac (i.e. linux) define GLEW
linux-clang* {
                DEFINES += LINUX
                LIBS+= -lGLEW
}
linux-g++-* {
                DEFINES += LINUX
                LIBS+= -lGLEW
}
DEPENDPATH+=include
# if we are on a mac define DARWIN
macx:DEFINES += DARWIN

# this is where to look for includes
INCLUDEPATH += $$(HOME)/NGL/include/

win32: {
                                DEFINES+=USING_GLEW

                                INCLUDEPATH+=-I c:\boost_1_40_0
                                INCLUDEPATH+=-I c:/SDL-1.2.14/include
                                INCLUDEPATH+=-I c:/SDL-1.2.14/include/SDL
                                INCLUDEPATH+=-I c:/SDL_ttf-2.0.9/include
                                INCLUDEPATH+= -I C:/glew/include
                                LIBS+= -L C:/GraphicsLib/lib
                                LIBS+= -L C:/glew/bin
                                LIBS+= -lmingw32
                                LIBS+= -lglew32
}
