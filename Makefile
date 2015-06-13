CC=gcc
CFLAGS=--debug -g -Wall -Wextra -Wformat-security -std=gnu99 -I"src" -lm
SOURCES=src/main.c src/allocation.c src/map.c src/util.c
TERMBOXFILES=termbox/src/termbox.c termbox/src/utf8.c

all: src/main.c
	$(CC) -o tinyrl $(SOURCES) $(TERMBOXFILES) $(CFLAGS)

clean:
	rm -f ./tinyrl
