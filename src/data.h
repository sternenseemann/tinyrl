#include <stdint.h>

struct liveform {
	// position
	int x;
	int y;
	// lives of the liveform
	unsigned int lives;
	// what does it look like?
	unsigned int c;
	// what's its color?
	uint16_t color;
};

struct World {
	// the player
	struct liveform player;

	// array of monsters and their count
	struct liveform *monsters;
	unsigned int monsterc;

	// the two-dimensional map array and
	// its dimensions
	unsigned int map_dimensions[2];
	unsigned int **map;

	// the exit point of the current level
	unsigned int stairs[2];
	// the current level number
	unsigned int level;
};
