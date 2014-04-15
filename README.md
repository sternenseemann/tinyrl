tinyRL
======

![](https://pbs.twimg.com/media/BlQUC65IEAAfLVC.png)

tinyRL is a tiny rogue-like written in C. 

## Playing
You will find yourself converted into a magenta `@` in a black enviroment surrounded by evil red `m`.  
But there's hope! Run to the `█` and escape! But watch out, the `m` will chase you and five of them together have the power to kill you.

Control the `@` using `hjkl` or the arrow keys. If you get bored of this game (you won't, trust me), you can exit with `ESC` or `q`.

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
