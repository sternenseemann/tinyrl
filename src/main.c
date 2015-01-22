#include "../termbox/src/termbox.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#define debug(M, ...) fprintf(stderr, "DEBUG %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

enum{
	FALSE = 0,
	TRUE = 1,
};

#define MAP_START_X 0
#define MAP_START_Y 1
#define MAP_END_X world->map_dimensions[0]
#define MAP_END_Y world->map_dimensions[1]

#define PLAYER_LIVES 9

// currently for player always a pointer is passed as a argument
// since monsters is an array it behaves like a pointer and you can
// modify its contents -> no need to pass it as a pointer
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
	struct liveform player;

	struct liveform *monsters;
	unsigned int monsterc;

	unsigned int map_dimensions[2];
	unsigned int **map;

	unsigned int stairs[2];
	unsigned int level;
};

/*
unsigned int **allocate_map(struct World *world) {
	unsigned int ** map = malloc(world->map_dimensions[0] * sizeof(unsigned int*));
	unsigned int i;

	assert(map != NULL);

	map[0] = malloc(world->map_dimensions[1] * world->map_dimensions[0] * sizeof(unsigned int));
	assert(map[0] != NULL);

	for (i = 1; i < world->map_dimensions[0]; i++) {
		map[i] = map[0] + (i * world->map_dimensions[1] * sizeof(unsigned int));
		debug("Allocated Collum %d\n", i);
	}
	return map;
} */

unsigned int **allocate_map(struct World *world) {
	unsigned int ** map = malloc(world->map_dimensions[0] * sizeof(unsigned int *));
	unsigned int i;
	for (i = 0; i < world->map_dimensions[0]; i++) {
		map[i] = malloc(world->map_dimensions[1] * sizeof(unsigned int));
	}

	return map;
}

void free_map(struct World *world) {
	assert(world->map != NULL);
	unsigned int i;
	for(i = 0; i < world->map_dimensions[0]; i++) {
		assert(world->map[i] != NULL);
		free(world->map[i]);
	}

	free(world->map);
}

int randint(int lower, int upper) {
	return rand() % upper + lower;
}

void generate_map(struct World *world) {
	// intialize the map by filling it with spaces
	unsigned int mapx;
	unsigned int mapy;
	for(mapx = 0; mapx < world->map_dimensions[0]; mapx++) {
		for(mapy = 0; mapy < world->map_dimensions[1]; mapy++)
		{
			world->map[mapx][mapy] = ' ';
		}
	}
	// generate map
	// map is a array of arrays:
	// map[map_dimensions[0]][map_dimensions[1]]
	//
	// fields are accesed like map[x][y] (which could be '.' or '#' etc.)

	// generate a house
	// looks like that:
	// ┌-- The coordinates for this brick are randomly chosen as a base for generation.
	// v
	// #####
	// #   #
	// #   #
	// #   #
	// #   #
	// ## ##
	//
	// the start brick needs min. 5 chars distance to the terminal border (bottom and right one)
	// indexes start with 0 -> we need to subtract 6
	int house_start[2];
	// Generate min. 1 house
	int housec = randint(1, 3);

	while(housec > 0) {
		house_start[0] = randint(MAP_START_X, (MAP_END_X - MAP_START_X - 6));
		house_start[1] = randint(MAP_START_Y, (MAP_END_Y - MAP_START_Y - 6));

		int housex = house_start[0];
		int housey = house_start[1];

		// add the vertical walls
		for(housey = house_start[1]; housey < (house_start[1] + 5); housey++) {
			// left wall
			world->map[housex][housey] = '#';
			// right wall
			world->map[housex + 4][housey] = '#';
		}
		// add the horizontal walls
		for(housex = house_start[0]; housex < (house_start[0] + 5); housex++) {
			// top
			world->map[housex][housey - 5] = '#';
			// bottom, the if is for the empty cell to get in
			if(housex != house_start[0] + 2) {
				world->map[housex][housey] = '#';
			}
		}
		--housec;
	}

	// fill the rest with ground ('.')
	for(mapx = MAP_START_X; mapx < MAP_END_X; mapx++) {
		for(mapy = MAP_START_Y; mapy < MAP_END_Y; mapy++) {
			if(world->map[mapx][mapy] != '#') {
				world->map[mapx][mapy] = '.';
			}
		}

	}
}

void draw(struct World *world) {
	// clear the screen
	tb_clear();

	// walk trough the map...
	unsigned int mapx;
	unsigned int mapy;
	for(mapx = 0; mapx < world->map_dimensions[0]; mapx++) {
		for(mapy = 0; mapy < world->map_dimensions[1]; mapy++) {
			// ...and draw the map
			tb_change_cell(mapx,mapy,world->map[mapx][mapy],TB_WHITE,TB_DEFAULT);
		}

	}

	// draw the status bar
	char level_str[100];
	char lives_str[100];
	sprintf(level_str, "Level: %d |", world->level);
	sprintf(lives_str, "Lives: %d/%d", world->player.lives, PLAYER_LIVES);

	int stri;
	int level_str_end;
	for(stri = 0; level_str[stri] != '\0'; stri++) {
		tb_change_cell(0 + stri, 0, (uint32_t) level_str[stri], TB_WHITE, TB_DEFAULT);
	}

	// start drawing from the end of the level string + 1 space
	level_str_end = stri + 1;

	for(stri = 0; lives_str[stri] != '\0'; stri++) {
		tb_change_cell(0 + level_str_end + stri, 0, (uint32_t) lives_str[stri], TB_WHITE, TB_DEFAULT);
	}

	// draw the stairs to the next level
	tb_change_cell(world->stairs[0], world->stairs[1], '>',TB_WHITE,TB_DEFAULT);

	// draw the monsters
	unsigned int i;
	for(i = 0; i < world->monsterc; i++) {
		// the monster hasn't been put on the "graveyard" in (-1,-1)
		if(world->monsters[i].x != - 1 && world->monsters[i].y != - 1) {
			tb_change_cell(world->monsters[i].x,world->monsters[i].y, world->monsters[i].c, world->monsters[i].color, TB_DEFAULT);
		}
	}

	// draw the player!
	tb_change_cell(world->player.x, world->player.y, world->player.c, world->player.color, TB_DEFAULT);
	// present the screen to the player
	tb_present();
}

int test_position(int x, int y, struct World *world) {
	// is the position in the terminal? Is there no '#'? Is there no player (needed for the better fighting mechanism)
	if(x >= MAP_START_X && x < (int) MAP_END_X && y >= MAP_START_Y && y < (int) MAP_END_Y &&
		world->map[x][y] != '#' && (world->player.x != x || world->player.y != y)) {
		return 1;
	} else {
		return 0;
	}
}

// returns - 1 if false; otherwise the index of the monster in monsters
int test_for_monsters(int x, int y, struct World *world) {
	// walk trough the monsters and check if one is at the specific point
	for(unsigned int i = 0; i < world->monsterc; i++) {
		if(world->monsters[i].x == x && world->monsters[i].y == y) {
			return i;
		}
	}
	return - 1;
}

void fight(int monsteri, struct World *world) {
	// take one live of the player and the monster
	world->player.lives -= 1;
	world->monsters[monsteri].lives -= 1;

	// if the monster is dead (0 lives)
	if(world->monsters[monsteri].lives == 0) {
		world->monsters[monsteri].x = -1; // put the monster into the "graveyard"
		world->monsters[monsteri].y = -1; // monsters at (-1,-1) are simply ignored 8)
	}
}

void handle_move(int new_x, int new_y, struct World *world) {
	int monster_there = test_for_monsters(new_x, new_y, world);

	// position is in the terminal and there's no monster -> move there
	if(test_position(new_x, new_y, world) == 1 && monster_there == - 1) {
		world->player.x = new_x;
		world->player.y = new_y;

	// there's a monster -> fight
	} else if(test_position(new_x, new_y, world) == 1 && monster_there != - 1) {
		fight(monster_there, world);
	}
}

void move(uint16_t key, uint32_t ch, struct World *world) {
	int new_x = world->player.x;
	int new_y = world->player.y;

	if(key == TB_KEY_ARROW_UP || ch == 'k') {
		new_x = world->player.x;
		new_y = world->player.y - 1;
	} else if(key == TB_KEY_ARROW_DOWN || ch == 'j') {
		new_x = world->player.x;
		new_y = world->player.y + 1;
	} else if(key == TB_KEY_ARROW_LEFT || ch == 'h') {
		new_x = world->player.x - 1;
		new_y = world->player.y;
	} else if(key == TB_KEY_ARROW_RIGHT || ch == 'l') {
		new_x = world->player.x + 1;
		new_y = world->player.y;
	}

	handle_move(new_x, new_y, world);
}

void move_monsters(struct World *world) {
	unsigned int i;
	for(i = 0; i < world->monsterc; i++) {
		// is the monster on the "graveyard" at (-1,-1)
		if(world->monsters[i].x == -1 || world->monsters[i].y == -1) {
			continue;
		}
		// calculate x-distance and y-distance
		int xdist = world->monsters[i].x - world->player.x;
		int ydist = world->monsters[i].y - world->player.y;

		// is there no way to go?
		int nulldist = (ydist == 0) && (xdist == 0);

		int newx, newy;

		if(ydist > 0 && ydist >= xdist && !nulldist) {
			newy = world->monsters[i].y - 1;
			if(test_position(world->monsters[i].x, newy, world) && test_for_monsters(world->monsters[i].x, newy, world) == -1) {
				world->monsters[i].y = newy;
			}
		} else if(ydist < 0 && ydist < xdist && !nulldist) {
			newy = world->monsters[i].y + 1;
			if(test_position(world->monsters[i].x, newy, world) && test_for_monsters(world->monsters[i].x, newy, world) == -1) {
				world->monsters[i].y = newy;
			}
		} else if(xdist > 0 && xdist >= ydist && !nulldist) {
			newx = world->monsters[i].x - 1;
			if(test_position(newx, world->monsters[i].y, world) && test_for_monsters(newx, world->monsters[i].y, world) == -1) {
				world->monsters[i].x = newx;
			}
		} else if(xdist < 0 && xdist < ydist && !nulldist) {
			newx = world->monsters[i].x + 1;
			if(test_position(newx, world->monsters[i].y, world) && test_for_monsters(newx, world->monsters[i].y, world) == -1) {
				world->monsters[i].x = newx;
			}
		}


		int newxdist = world->monsters[i].x - world->player.x;
		int newydist = world->monsters[i].y - world->player.y;

		// distance is <= 1 and player and monster share either the same x or y
		if( (abs(newxdist) <= 1) && ( abs(newydist) <= 1) && ( world->monsters[i].x == world->player.x || world->monsters[i].y == world->player.y)) {
			fight(i, world);
		}
	}
}


int main(void) {
	// loop control
	int exit = FALSE;
	// intialize world struct
	struct World *world = malloc(sizeof(struct World));
	assert(world != NULL);

	// intialize the player struct
	world->player.lives = PLAYER_LIVES;
	world->player.c = '@';
	world->player.color = TB_WHITE;
	// level
	world->level = 1;

	// here will our events be stored
	struct tb_event event;

	// start termbox
	int err = tb_init();
	assert(!err);

	// use normal output mode (maybe 256-Color soon?)
	tb_select_output_mode(TB_OUTPUT_NORMAL);

	// seed our shitty RNG
	time_t seed = time(NULL);
	srand(seed);

	// level generation loop
	do {
		// reset the player's location
		world->player.x = MAP_START_X;
		world->player.y = MAP_START_Y;

		// init the size of the map. The map does NOT resize
		world->map_dimensions[0] = tb_width();
		world->map_dimensions[1] = tb_height();

		world->map = allocate_map(world);
		generate_map(world);

		// pick a random location for the exit
		// that is accesible to the player
		do {
			world->stairs[0] = randint(MAP_START_X, (MAP_END_X - MAP_START_X));
			world->stairs[1] = randint(MAP_START_Y, (MAP_END_Y - MAP_START_Y));
		}while(!test_position(world->stairs[0], world->stairs[1], world));

		// random count of monsters
		world->monsterc = randint(0, 6);
		world->monsters = malloc(world->monsterc * sizeof(struct liveform));
		assert(world->monsters != NULL);

		unsigned int i;
		for(i = 0; i < world->monsterc; i++) {
			world->monsters[i].x = randint(MAP_START_X, (MAP_END_X - MAP_START_X));
			world->monsters[i].y = randint(MAP_START_Y, (MAP_END_Y - MAP_START_Y));
			world->monsters[i].lives = randint(1, 2);

			if(world->monsters[i].lives > 1) {
				// it's an ork and more dangerous
				world->monsters[i].c = 'o';
				world->monsters[i].color = TB_GREEN;
			} else {
				// it's "only" a warg
				world->monsters[i].c = 'w';
				world->monsters[i].color = TB_CYAN;
			}
		}

		// game event loop
		while(!exit) {
			draw(world);
			tb_poll_event(&event); // wait for an event

			switch(event.type) {
				case TB_EVENT_KEY: // a key got pressed
					switch(event.key) {
						case TB_KEY_CTRL_C:
						case TB_KEY_CTRL_D:
						case TB_KEY_ESC:
							exit = TRUE;
							break;
						default: // let the move-function check if we have to move or not
							move(event.key, event.ch, world);
							// then move the monsters
							move_monsters(world);
							break;
					}

					// ugly but
					// using tb_event and switch statements is a bit annoying
					if(event.ch == 'q') {
						exit = TRUE;
					}
					break;
				case TB_EVENT_RESIZE:
					// resizing is basically cheating, so we quit :)
					exit = TRUE;
					break;
			}

			// did we reach the stairs to the next level?
			if(world->player.x == (int) world->stairs[0] && world->player.y == (int) world->stairs[1]) {
				// break out of the level loop
				// but not out of the level generation loop
				break;
			}

			// are we dead?
			if(world->player.lives <= 0) {
				exit = 1;
			}
		}

		// if we don't exit the game
		// increase the level counter
		if(!exit) {
			world->level++;
		}
	}while(!exit);

	tb_shutdown();

	// free all the stuff!
	free_map(world);
	free(world->monsters);
	free(world);

	return EXIT_SUCCESS;
}
