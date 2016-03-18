CC=g++
CFLAGS=-c -Wall -fPIC -std=c++11
SOURCES=Scheduler/scheduler.cpp Polynom/polynom.cpp Polynom/checkIrreducible.cpp
OBJECTS=$(SOURCES:.cpp=.o)
LIB=../../build/libirredpoly.so${LIBVER}
LIBVER=
LDFLAGS=-ldl

all:$(SOURCES) $(LIB) clean


$(LIB): $(OBJECTS)
	$(CC) -shared -fPIC -o $(LIB) $(OBJECTS)
	
.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o */*.o