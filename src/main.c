#include "../termbox/termbox.h"
#include <stdio.h>
#include <stdlib.h> // for rand
#include <time.h> // for time()
#include <math.h> // for abs

#define debug(M, ...) fprintf(stderr, "DEBUG %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

enum{
	FALSE = 0,
	TRUE = 1,
};


int map_dimensions[2];

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
};

void generate_map(int map[map_dimensions[0]][map_dimensions[1]])
{
	extern int map_dimensions[2];
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
	int housec = 1 + ( rand() % 3);

	while(housec > 0){
		house_start[0] = rand() % ( map_dimensions[0] - 6 ); 
		house_start[1] = rand() % ( map_dimensions[1] - 6 );

		int housex = house_start[0];
		int housey = house_start[1];

		// add the vertical walls
		for(housey = house_start[1]; housey < (house_start[1] + 5); housey++)
		{
			// left wall
			map[housex][housey] = '#';
			// right wall
			map[housex + 4][housey] = '#';
		}
		// add the horizontal walls
		for(housex = house_start[0]; housex < (house_start[0] + 5); housex++)
		{
			// top
			map[housex][housey - 5] = '#';
			// bottom, the if is for the empty cell to get in
			if(housex != house_start[0] + 2){
				map[housex][housey] = '#';
			}
		}
		--housec;
	}

	// fill the rest with ground ('.')
	int mapx;
	for(mapx = 0; mapx < map_dimensions[0]; mapx++)
	{
		int mapy;
		for(mapy = 0; mapy < map_dimensions[1]; mapy++){
			if(map[mapx][mapy] != '#')
			{
				map[mapx][mapy] = '.';
			}
		}

	}
}

void draw(int map[map_dimensions[0]][map_dimensions[1]], struct liveform *player, int stairs[2], int monsterc, struct liveform monsters[])
{
	extern int map_dimensions[2];

	tb_clear();

	int mapx;
	for(mapx = 0; mapx < map_dimensions[0]; mapx++)
	{
		int mapy;
		for(mapy = 0; mapy < map_dimensions[1]; mapy++){
			tb_change_cell(mapx,mapy,map[mapx][mapy],TB_WHITE,TB_DEFAULT);
		}

	}

	tb_change_cell(tb_width() - 1, 0, player->lives + '0', TB_WHITE, TB_DEFAULT);
	for(int i = 0; i < monsterc; i++)
	{
		// the monster hasn't been put on the "graveyard" in (-1,-1)
		if(monsters[i].x != - 1 && monsters[i].y != - 1) 
		{
			tb_change_cell(monsters[i].x,monsters[i].y, monsters[i].c, TB_RED, TB_DEFAULT);
		}
	}

	tb_change_cell(player->x, player->y, player->c, TB_WHITE, TB_DEFAULT);
	tb_change_cell(stairs[0], stairs[1], '>',TB_WHITE,TB_DEFAULT);

	tb_present();
}

int test_position(int x, int y, struct liveform *player, int map[map_dimensions[0]][map_dimensions[1]])
{
	extern int map_dimensions[2];
	// is the position in the terminal? Is there no '#'? Is there no player (needed for the better fighting mechanism)
	if(x >= 0 && x < tb_width() && y >= 0 && y < tb_height() && map[x][y] != '#' && (player->x != x || player->y != y)){ 
		return 1;
	}else{
		return 0;
	}
}

// returns - 1 if false; otherwise the index of the monster in monsters
int test_for_monsters(int x, int y, int monsterc, struct liveform monsters[]) 
{
	// walk trough the monsters and check if one is at the specific point
	for(int i = 0; i < monsterc; i++)
	{
		if(monsters[i].x == x && monsters[i].y == y)
		{
			return i;
		}
	}
	return - 1;
}

void fight(struct liveform *player, int monsteri, struct liveform monsters[])
{
	// take one live of the player and the monster
	player->lives -= 1;
	monsters[monsteri].lives -= 1;

	// if the monster is dead (0 lives)
	if(monsters[monsteri].lives == 0)
	{
		monsters[monsteri].x = -1; // put the monster into the "graveyard"
		monsters[monsteri].y = -1; // monsters at (-1,-1) are simply ignored 8)
	}
}

void handle_move(int new_x, int new_y, struct liveform *player, int monsterc, struct liveform monsters[], int map[map_dimensions[0]][map_dimensions[1]])
{
	extern int map_dimensions[2];

	int monster_there = test_for_monsters(new_x, new_y, monsterc, monsters);

	// position is in the terminal and there's no monster -> move there
	if(test_position(new_x, new_y, player, map) == 1 && monster_there == - 1)
	{ 
		player->x = new_x;
		player->y = new_y;
	}
	// there's a monster -> fight
	else if(test_position(new_x, new_y, player, map) == 1 && monster_there != - 1) 
	{
		fight(player, monster_there, monsters); 
	}
}

void move(struct liveform *player, uint16_t key, uint32_t ch, int monsterc, struct liveform monsters[], int map[map_dimensions[0]][map_dimensions[1]])
{
	extern int map_dimensions[2];

	int new_x = player->x;
	int new_y = player->y;

	if(key == TB_KEY_ARROW_UP || ch == 'k')
	{
		new_x = player->x;
		new_y = player->y - 1;
	}
	else if(key == TB_KEY_ARROW_DOWN || ch == 'j')
	{
		new_x = player->x;
		new_y = player->y + 1;
	}
	else if(key == TB_KEY_ARROW_LEFT || ch == 'h')
	{
		new_x = player->x - 1;
		new_y = player->y;
	}
	else if(key == TB_KEY_ARROW_RIGHT || ch == 'l')
	{
		new_x = player->x + 1;
		new_y = player->y;
	}

	handle_move(new_x, new_y, player, monsterc, monsters, map);
}

void move_monsters(struct liveform *player, int monsterc, struct liveform monsters[], int map[map_dimensions[0]][map_dimensions[1]])
{
	for(int i = 0; i < monsterc; i++)
	{
		// is the monster on the "graveyard" at (-1,-1)
		if(monsters[i].x == -1 || monsters[i].y == -1)
		{
		     continue;
		}
		// calculate x-distance and y-distance
		int xdist = monsters[i].x - player->x;
		int ydist = monsters[i].y - player->y;

		// is there no way to go?
		int nulldist = (ydist == 0) && (xdist == 0); 

		int newx, newy;

		if(ydist > 0 && ydist >= xdist && !nulldist)
		{
			newy = monsters[i].y - 1;
			if(test_position(monsters[i].x, newy, player, map) && test_for_monsters(monsters[i].x, newy, monsterc, monsters) == -1)
			{
				monsters[i].y = newy; 
			}
		}
		else if(ydist < 0 && ydist < xdist && !nulldist)
		{
			newy = monsters[i].y + 1;
			if(test_position(monsters[i].x, newy, player, map) && test_for_monsters(monsters[i].x, newy, monsterc, monsters) == -1)
			{
				monsters[i].y = newy; 
			}
		}
		else if(xdist > 0 && xdist >= ydist && !nulldist)
		{
			newx = monsters[i].x - 1;
			if(test_position(newx, monsters[i].y, player, map) && test_for_monsters(newx, monsters[i].y, monsterc, monsters) == -1)
			{
				monsters[i].x = newx;
			}
		}
		else if(xdist < 0 && xdist < ydist && !nulldist)
		{
			newx = monsters[i].x + 1;
			if(test_position(newx, monsters[i].y, player, map) && test_for_monsters(newx, monsters[i].y, monsterc, monsters) == -1)
			{
				monsters[i].x = newx;
			}
		}


		int newxdist = monsters[i].x - player->x;
		int newydist = monsters[i].y - player->y;

		// distance is <= 1 and player and monster share either the same x or y
		if( (abs(newxdist) <= 1) && ( abs(newydist) <= 1) && ( monsters[i].x == player->x || monsters[i].y == player->y))
		{
			fight(player, i, monsters);
		}
	}
}

int main(void)
{
	extern int map_dimensions[2];

	// controller of the main loops
	int exit = FALSE;
	// did we win or die?
	int won = FALSE;
	// do we want to save?
	int save = FALSE;

	// level counter
	int level = 1;

	// intialize the player struct
	struct liveform player;
	player.lives = 9;
	player.c = '@';

	// { x-pos of the stairs to the next level, y-pos of stairs }
	int stairs[2]; 

	// here will our events be stored
	struct tb_event event; 

	// start termbox
	tb_init();

	// seed our shitty RNG
	time_t seed = time(NULL);
	srand(seed);

	do{
		// reset the player's location
		player.x = 0;
		player.y = 0;

		// init the size of the map. The map does NOT resize
		map_dimensions[0] = tb_width();
		map_dimensions[1] = tb_height();

		// create an array of the size
		// see generate_map for a further explanition
		// of the data structure
		int map[map_dimensions[0]][map_dimensions[1]];

		generate_map(map);

		// pick a random location for the exit
		// that is accesible to the player
		do{
			stairs[0] = rand() % tb_width();
			stairs[1] = rand() % tb_height();
		}while(!test_position(stairs[0], stairs[1], &player, map));

		// random count of monsters
		int monsterc = rand() % 10;
		// monsters[n] = { x-position, y-position, lives }
		struct liveform monsters[monsterc];

		for(int i = 0; i < monsterc; i++)
		{
			monsters[i].x = rand() % tb_width();
			monsters[i].y = rand() % tb_height();
			monsters[i].lives = 1 + ( rand() % 2);
			monsters[i].c = 'm';
		}

		while(!exit)
		{
			draw(map, &player, stairs, monsterc, monsters);
			tb_poll_event(&event); // wait for an event

			switch(event.type)
			{
				case TB_EVENT_KEY: // a key got pressed
					switch(event.key)
					{
						case TB_KEY_CTRL_C:
						case TB_KEY_CTRL_D:
						case TB_KEY_ESC: 
							exit = TRUE;
							save = TRUE;
							won = FALSE;
							break;
						default: // let the move-function check if we have to move or not
							move(&player,event.key, event.ch, monsterc, monsters, map);
							// then move the monsters
							move_monsters(&player, monsterc, monsters, map);
							break;
					}

					// this doesn't feel right,
					// using tb_event and switch statements is a bit annoying
					if(event.ch == 'q')
					{
						exit = TRUE;
						save = TRUE;
						won = FALSE;
					}
					break;
			}

			// did we reach the stairs to the next level?
			if(player.x == stairs[0] && player.y == stairs[1])
			{
				// break out of the level loop
				// but not out of the level generation loop
				break;
			}

			// are we dead? 
			if(player.lives == 0){
				exit = 1;
				won = FALSE;
			}
		}

		// if we don't exit the game
		// increase the level counter
		if(!exit)
		{
			level++;
		}

	}while(!exit);

	tb_shutdown();

	if(save)
	{
		// call save routine and so on
		// is a TODO
		printf("You saved at level %d of the dungeon\n", level);
	}
	else if(won)
	{
		printf("\\o/ You won at level %d of the dungeon\n", level);
	}else{
		printf("You died at level %d of the dungeon\n", level);
	}

	return 0;
}
