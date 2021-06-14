C?=gcc
CXX?=g++
DEBUG=-DDEBUG -g
INCLUDES=-I..
CFLAGS=-Wall -O3 -std=c++11 $(DEBUG) $(INCLUDES)
#CFLAGS=-Wall $(DEBUG) $(INCLUDES)
LDFLAGS=
VPATH=../src

OBJECTS= \
  M6502.o \
  MemoryBus.o \
  Television.o \
  VNC.o

default: $(OBJECTS)
	$(CXX) -o ../moln_spelare ../src/moln_spelare.cxx \
	  $(OBJS) \
	  $(CFLAGS) $(LDFLAGS)

%.o: %.cxx %.h
	$(CXX) -c $< -o $*.o \
	  $(CFLAGS)

