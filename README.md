tinyRL
======

![](https://i.imgur.com/mn1IXxQ.png)

tinyRL is a tiny rogue-like written in C. 

## Playing
You will find yourself converted into a magenta `@` in a black enviroment surrounded by evil red `m`.  
But there's hope! Run to the `>` and escape! But watch out, the `m` will chase you and five of them together have the power to kill you.

## Building
Should compile on most unixes, use a terminal emulator with UTF-8 support.

	git clone https://github.com/lukasepple/tinyrl.git
	cd tinyrl
	make
	./bin/tinyrl # have fun!


## robot rogue patch
This roguelike is a attempt to rewrite [robot-rogue](https://github.com/lukasepple/robot-rogue).

To turn tinyRL into robot-rogue I created the patch `robot-rogue.patch`. Apply it by `patch < robot-rogue.patch` and then entering `src/main.c`.

Now run `make` and execute `./bin/tinyrl`. The story is different:

> Once there was a little robot who was sent to earth to check if the humans could live there again and to find Richard-Edward's cat again.
