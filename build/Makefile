C?=gcc
CXX?=g++
DEBUG=-DDEBUG -g
INCLUDES=-I..
#OPT=-mcpu=cortex-a72 -mtune=cortex-a72
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
  RIOT.o \
  ROM.o \
  TIA.o \
  Television.o \
  TelevisionHttp.o \
  TelevisionNull.o \
  TelevisionVNC.o

default: $(OBJECTS) TelevisionSDL.o
	$(CXX) -o ../cloudtari ../src/cloudtari.cxx \
	  $(OBJECTS) TelevisionSDL.o \
	  $(CFLAGS) $(LDFLAGS) -DUSE_SDL

nosdl: $(OBJECTS)
	$(CXX) -o ../cloudtari ../src/cloudtari.cxx \
	  $(OBJECTS) \
	  $(CFLAGS)

%.o: %.cxx %.h
	$(CXX) -c $< -o $*.o \
	  $(CFLAGS)

