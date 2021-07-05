C?=gcc
CXX?=g++
DEBUG=-DDEBUG -g
INCLUDES=-I..
CFLAGS=-Wall -O3 -std=c++11 $(DEBUG) $(INCLUDES)
#CFLAGS=-Wall $(DEBUG) $(INCLUDES)
LDFLAGS=-lSDL2
VPATH=../src

OBJECTS= \
  ColorTable.o \
  Disassembler.o \
  GifCompressor.o \
  M6502.o \
  MemoryBus.o \
  Network.o \
  PIA.o \
  ROM.o \
  TIA.o \
  Television.o \
  TelevisionHttp.o \
  TelevisionNull.o \
  TelevisionSDL.o \
  TelevisionVNC.o

default: $(OBJECTS)
	$(CXX) -o ../cloudtari ../src/cloudtari.cxx \
	  $(OBJECTS) \
	  $(CFLAGS) $(LDFLAGS)

%.o: %.cxx %.h
	$(CXX) -c $< -o $*.o \
	  $(CFLAGS)

