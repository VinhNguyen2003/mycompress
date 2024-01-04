CC=gcc
CFLAGS=-Wall -g -Iincludes

SOURCES=source/tar_handler.c source/file_list.c source/zip_handler.c source/7z_handler.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=mycompress

all: $(EXECUTABLE)

$(EXECUTABLE): main.o $(OBJECTS)
	$(CC) $(CFLAGS) main.o $(OBJECTS) -o $@ -lz -lzip -larchive

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

source/%.o: source/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf source/*.o main.o $(EXECUTABLE)
