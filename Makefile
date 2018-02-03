CC	= g++
CPPFLAGS= -Wno-deprecated -O3 -c
LDFLAGS	= -O3
SOURCES	= tga.cpp
OBJECTS	= $(SOURCES:.cpp=.o)
EXECUTABLE=tga

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE) : $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o : 
	$(CC) $(CPPFLAGS) $< -o $@

clean:
	rm -f *.o