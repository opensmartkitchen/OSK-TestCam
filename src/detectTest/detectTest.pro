TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

#LIBS += -L/usr/local/lib
LIBS += -lopencv_core -lopencv_highgui -lopencv_videoio -lopencv_imgproc -lopencv_video

SOURCES += main.cpp
