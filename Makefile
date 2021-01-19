TARGET = numlocknotify
CC = gcc
CFLAGS = -g -Wall
INCLUDES = \
					 $(shell pkg-config --cflags glib-2.0) \
					 $(shell pkg-config --cflags gtk+-3.0) \
					 $(shell pkg-config --cflags libnotify)

LIBS = -lX11 \
				$(shell pkg-config --libs glib-2.0) \
				$(shell pkg-config --libs gtk+-3.0) \
				$(shell pkg-config --libs libnotify)

.PHONY: all default clean strip

default: $(TARGET)
all: default strip

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
		$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
		$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
		-rm -f *.o
		-rm -f $(TARGET)

strip:
		strip $(TARGET)
