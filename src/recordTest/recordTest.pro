TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lopencv_core -lopencv_highgui -lopencv_videoio

SOURCES += main.cpp
