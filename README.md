tinyrl
======

![](https://i.imgur.com/mn1IXxQ.png)

tiny rogue-like written in C. Its also a try to rewrite robot-rogue in C because my Object structure is totally broken.

## Playing
You will find yourself converted into a magenta `@` in a black enviroment surrounded by evil red `m`.  
But there's hope! Run to the `>` and escape! But watch out, the `m` will chase you and five of them together have the power to kill you.

## Building
`make` it.

## robot rogue patch
As mentioned above this roguelike is a attempt to rewrite [robot-rogue](https://github.com/lukasepple/robot-rogue).

To turn tinyrl into robot-rogue I created the patch `robot-rogue.patch`. Apply it by `patch < robot-rogue.patch` and then entering `src/main.c`.

Now run `make` and execute `./bin/tinyrl`. The story is different:

> Once there was a little robot who was sent to earth to check if the humans could live there again and to find Richard-Edward's cat again.
