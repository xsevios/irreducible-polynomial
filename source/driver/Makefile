CC=g++
CFLAGS=-c -Wall -std=c++11
SOURCES=main.cpp LibraryInterface/LibraryInterface.cpp Config/config.cpp
OBJECTS=main.o LibraryInterface/LibraryInterface.o Config/config.o
BIN=../../build/driver
LDFLAGS=-ldl

all:$(SOURCES) $(BIN) clean

$(BIN):$(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(BIN)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

#lib.o:
#	g++ -с lib/lib.cpp
	
clean:
	rm -f *.o */*.o