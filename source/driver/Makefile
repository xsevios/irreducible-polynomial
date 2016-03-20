CC=g++
CFLAGS=-c -Wall -std=c++11
SOURCES=main.cpp driver.cpp LibraryInterface/LibraryInterface.cpp Config/config.cpp
OBJECTS=$(SOURCES:.cpp=.o)
BIN=../../build/driver
LDFLAGS=-ldl
DFLAGS = -MD

all:$(SOURCES) $(BIN) clean

$(BIN):$(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(BIN)

-include $(OBJECTS:.o=.d)

.%.o : %.cpp
	$(CC) $(CFLAGS) $(DFLAGS) $< -o $@

#lib.o:
#	g++ -с lib/lib.cpp
	
clean:
	rm -f *.o */*.o