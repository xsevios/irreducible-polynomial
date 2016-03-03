CC=g++
CFLAGS=-c -Wall -fPIC
SOURCES=lib.cpp
OBJECTS=$(SOURCES:.cpp=.o)
LIB=../../build/libirredpoly.so${LIBVER}
LIBVER=
LDFLAGS=-ldl

all:$(SOURCES) $(LIB) clean


$(LIB): $(OBJECTS)
	g++ -shared -fPIC -o $(LIB) $(OBJECTS)
	
.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o */*.o