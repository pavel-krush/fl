BINARY=fl

LDFLAGS=
CFLAGS=-g -O2 -Wall -Wno-unused-result

OBJECTS=usage.o commands.o file.o fl.o

all: $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(BINARY)

clean:
	rm -f *.o $(BINARY)
