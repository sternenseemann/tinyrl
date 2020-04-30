PREFIX=/usr/local

CC=gcc
CFLAGS=--debug -g -Wall -Wextra -Wformat-security -std=gnu99 -I"src" -lm -ltermbox
SOURCES=src/main.c src/allocation.c src/map.c src/util.c
TERMBOXFILES=termbox/src/termbox.c termbox/src/utf8.c

all: src/main.c
	$(CC) -o tinyrl $(SOURCES) $(CFLAGS)

install:
	install -D -m755 tinyrl $(DESTDIR)$(PREFIX)/bin/tinyrl

clean:
	rm -f ./tinyrl
