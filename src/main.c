#include "termbox.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "config.h"     // special configuration using #defines
#include "data.h"       // everything related to our special data structures
#include "map.h"        // generation of the map
#include "allocation.h" // everything related to allocation, freeing and exiting gracefully
#include "util.h"       // utility functions

#define debug(M, ...) fprintf(stderr, "DEBUG %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

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
	// is the position in the terminal? Is there no Wall? Is there no player (needed for the better fighting mechanism)
	if(x >= MAP_START_X && x < (int) MAP_END_X && y >= MAP_START_Y && y < (int) MAP_END_Y &&
		world->map[x][y] != WALL && world->map[x][y] != PARTLY_DESTRUCTED_WALL &&
		(world->player.x != x || world->player.y != y)) {
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

void destruct_wall(unsigned int x, unsigned int y, struct World *world) {
	if(abs(world->player.x - x) <= 1 && abs(world->player.y - y) <= 1) {
		switch(world->map[x][y]) {
			case WALL:
				world->map[x][y] = PARTLY_DESTRUCTED_WALL;
				break;
			case PARTLY_DESTRUCTED_WALL:
				world->map[x][y] = GROUND;
				break;
		}
	}
}

void handle_move(unsigned int new_x, unsigned int new_y, struct World *world) {
	int monster_there = test_for_monsters(new_x, new_y, world);

	// position is in the terminal and there's no monster -> move there
	if(test_position(new_x, new_y, world) == 1 && monster_there == - 1) {
		world->player.x = new_x;
		world->player.y = new_y;

	// there's a monster -> fight
	} else if(test_position(new_x, new_y, world) == 1 && monster_there != - 1) {
		fight(monster_there, world);
	} else if((int) new_x >= MAP_START_X && new_x < MAP_END_X &&
				(int) new_y >= MAP_START_Y && new_y < MAP_END_Y &&
				(world->map[new_x][new_y] == WALL ||
				world->map[new_x][new_y] == PARTLY_DESTRUCTED_WALL)) {
		destruct_wall(new_x, new_y, world);
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
	// wether to exit the loop
	int exit       = 0;
	// wether to regenerate the level
	int regenerate = 0;
	// intialize world struct
	struct World *world = malloc(sizeof(struct World));
	ensure(world != NULL, world);
	// make sure all pointers are NULL
	world->monsters = NULL;
	world->map      = NULL;

	// intialize the player struct
	world->player.lives = PLAYER_LIVES;
	world->player.c     = '@';
	world->player.color = TB_MAGENTA;

	// level
	world->level        = 1;

	// here will our events be stored
	struct tb_event event;

	// start termbox
	int err = tb_init();
	ensure(!err, world);

	// use normal output mode (maybe 256-Color soon?)
	tb_select_output_mode(TB_OUTPUT_NORMAL);

	// seed our shitty RNG
	time_t seed = time(NULL);
	srand(seed);

	// level generation loop
	do {
		// reset loop control variables
		exit       = 0;
		regenerate = 0;


		// if the map was allocated in
		// a previous level free it
		if(world->map != NULL)
			free_map(world);

		// init the size of the map
		// use the data from the
		// resize event because
		// it's up to date
		if(event.type == TB_EVENT_RESIZE) {
			world->map_dimensions[0] = event.w;
			world->map_dimensions[1] = event.h;
		} else {
			world->map_dimensions[0] = tb_width();
			world->map_dimensions[1] = tb_height();
		}

		// allocate the map
		// and generate it
		world->map = allocate_map(world);
		generate_map(world);


		// place the player
		do {
			world->player.x = randint(MAP_START_X, MAP_END_X);
			world->player.y = randint(MAP_START_Y, MAP_END_Y);
		}while(world->map[world->player.x][world->player.y] == WALL);

		// pick a random location for the exit
		// that is accesible to the player
		do {
			world->stairs[0] = randint(MAP_START_X, (MAP_END_X - MAP_START_X));
			world->stairs[1] = randint(MAP_START_Y, (MAP_END_Y - MAP_START_Y));
		}while(!test_position(world->stairs[0], world->stairs[1], world));

		// if the monster array was
		// already allocated previously
		// free it
		if(world->monsters != NULL)
			free(world->monsters);
		// random count of monsters
		world->monsterc = randint(0, MAX_MONSTERS);
		// allocate the array
		world->monsters = malloc(world->monsterc * sizeof(struct liveform));

		ensure(world->monsters != NULL, world);

		unsigned int i;
		for(i = 0; i < world->monsterc; i++) {
			world->monsters[i].x     = randint(MAP_START_X, (MAP_END_X - MAP_START_X));
			world->monsters[i].y     = randint(MAP_START_Y, (MAP_END_Y - MAP_START_Y));
			world->monsters[i].lives = randint(1, 2);

			if(world->monsters[i].lives > 1) {
				// it's an ork and more dangerous
				world->monsters[i].c     = 'o';
				world->monsters[i].color = TB_GREEN;
			} else {
				// it's "only" a warg
				world->monsters[i].c     = 'w';
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
							exit = 1;
							break;
						default: // let the move-function check if we have to move or not
							move(event.key, event.ch, world);
							// then move the monsters
							move_monsters(world);
							break;
					}

					switch(event.ch) {
						case 'q':
							exit = 1;
							break;
						default:
							break;
					}

					break;
				case TB_EVENT_RESIZE:
					// set regeneration control variable
					regenerate = 1;
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

			if(regenerate) {
				break;
			}
		}

		// if we don't exit the game
		// and don't regenerate
		// increase the level counter
		if(!exit && !regenerate) {
			world->level++;
		}
	} while(!exit);

	tb_shutdown();

	// free all the stuff!
	exit_gracefully(EXIT_SUCCESS, world);

	return 42; // we will never reach the answer
}
