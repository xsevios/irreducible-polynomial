CC=g++
CFLAGS=-c -Wall -fPIC -std=c++11
SOURCES=Scheduler/scheduler.cpp Polynom/polynom.cpp Polynom/checkIrreducible.cpp
OBJECTS=$(SOURCES:.cpp=.o)
LIB=../../build/libirredpoly.so${LIBVER}
LIBVER=
LDFLAGS=-ldl
DFLAGS=-MD -MP
PTHFLAG=-lpthread

all:$(LIB) clean

$(LIB): $(OBJECTS)
	$(CC) -shared -fPIC -o $(LIB) $(OBJECTS) $(PTHFLAG)
	
-include $(OBJECTS:.o=.d)

%.o: %.cpp
	$(CC) $(CFLAGS) $(PTHFLAG) $(DFLAGS) $< -o $@

clean:
	rm -f *.o */*.o */*.d *.d

.PHONY: all clean