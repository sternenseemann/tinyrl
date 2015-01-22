CC=gcc
CFLAGS=--debug -g -Wall -Wextra -Wformat-security -Werror -std=gnu99 -lm
TERMBOXFILES=termbox/src/termbox.c termbox/src/utf8.c

all: src/main.c 
	$(CC) -o tinyrl src/main.c $(TERMBOXFILES) $(CFLAGS)

clean:
	rm -f ./tinyrl
