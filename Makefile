CC       = gcc
CXX      = g++
CFLAGS   = -g
CXXFLAGS = $(CFLAGS)

LIBPATH = ./
LIBS = -Wl,-rpath=./:./HCNetSDKCom -lhcnetsdk -lPlayCtrl -lSuperRender -lAudioRender
SRC = ./main.cpp
TARGET = ./getpsdata

all: 
	$(CXX) $(OPTI) $(CXXFLAGS) $(SRC) -o $(TARGET) -L$(LIBPATH) $(LIBS)

.PHONY: clean
clean:
	rm -f $(TARGET)
