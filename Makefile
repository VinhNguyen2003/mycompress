CC=gcc
CFLAGS=-Wall -g

SOURCES=main.c tar_handler.c file_list.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=mycompress

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)
