Building For Kubernetes
=======================

Steps:

* Create a docker container using this Dockerfile (just type make here).
* In the container do:

    git clone https://github.com/mikeakohn/cloudtari.git

* Modify the source code to get rid of SDL:

    diff --git a/build/Makefile b/build/Makefile
    index a7e51ef..3f6b3c8 100644
    --- a/build/Makefile
    +++ b/build/Makefile
    @@ -5,7 +5,7 @@ INCLUDES=-I..
     #OPT=-mcpu=cortex-a72 -mtune=cortex-a72
     CFLAGS=-Wall -O3 -std=c++11 $(DEBUG) $(INCLUDES)
     #CFLAGS=-Wall $(DEBUG) $(INCLUDES)
    -LDFLAGS=-lSDL2
    +#LDFLAGS=-lSDL2
     VPATH=../src
     
     OBJECTS= \
    @@ -21,7 +21,6 @@ OBJECTS= \
       Television.o \
       TelevisionHttp.o \
       TelevisionNull.o \
    -  TelevisionSDL.o \
       TelevisionVNC.o
 
     default: $(OBJECTS)
    diff --git a/src/cloudtari.cxx b/src/cloudtari.cxx
    index ad24d2a..c9b0352 100644
    --- a/src/cloudtari.cxx
    +++ b/src/cloudtari.cxx
    @@ -19,7 +19,6 @@
     #include "ROM.h"
     #include "TelevisionHttp.h"
     #include "TelevisionNull.h"
    -#include "TelevisionSDL.h"
     #include "TelevisionVNC.h"
     #include "TIA.h"
 
    @@ -62,11 +61,6 @@ int main(int argc, char *argv[])
       m6502->set_memory_bus(memory_bus);
       m6502->reset();
     
    -  if (strcmp(argv[2], "sdl") == 0)
    -  {
    -    television = new TelevisionSDL();
    -  }
    -    else
       if (strcmp(argv[2], "vnc") == 0)
       {
         television = new TelevisionVNC();

* Type: make
* Copy the cloudtari executable that gets generated to:

    scripts/docker/

* Copy the 3 ROM files (or whatever ROM files) to scripts/docker and build that container.

