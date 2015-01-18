CC=gcc
CFLAGS=--debug -g -Wall -Wextra -Wformat-security -Werror -std=gnu99 -lm
TERMBOXFILES=termbox/src/termbox.c termbox/src/utf8.c

all: src/main.c 
	@mkdir -p bin
	$(CC) -o bin/tinyrl src/main.c $(TERMBOXFILES) $(CFLAGS)

clean:
	rm -rf bin/tinyrl
