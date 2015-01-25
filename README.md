tinyRL
======

![](./screenshot.png)

tinyRL is a tiny rogue-like written in C. 

## Building
Should compile on most unixes, use a terminal emulator with UTF-8 support.

	git clone https://github.com/lukasepple/tinyrl.git
	cd tinyrl
	git submodule update --init
	make
	./bin/tinyrl # have fun!

## Playing
Like in most roguelikes you'll be a `@` in a monster-filled enviroment. The interface is a 2D top-view representation of this enviroment.

On Top you'll see your current level and how many lives (or hitpoints) you have left.

You can control yourself using (vim users – listen up!) `hjkl` or the arrow keys. You can quit using `q`.

### Monsters
Currently there are two types:

* cyan `w`: A warg – a wolf-like creature with one live
* green `o`: A orc – he's two lives

## TODO
* implement an saving routine
* think about a real concept and adjust the map generation
* generate monsters and houses (or whatever) based on window size
* implement Field of Vision (FOV)
* implement Inventory
* ...

## Changelog
Newest Change always on top!

* The level gets regenerated when the window gets resized
* fixed several memory leaks and other bugs
* proper allocation error handling
* major refactoring, everything you now need to know is inside struct World
* dropped the broken saving stuff

## Thanks to

* [Lechindianer](https://github.com/Lechindianer) for fixing my Makefiles (although I have to fix the Makefiles again later)
* [plomlompom](https://github.com/plomlompom) who made me stumble upon roguelikes and roguelike development. And for his Informations about nearly everything.
* [vis7mac](https://github.com/vis7mac) who complains and opens issues
* [oderwat](https://github.com/oderwat) for complaining about my code style


