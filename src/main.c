#include "../termbox/termbox.h"
//#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int map_dimensions[2];

void generate_map(int map[map_dimensions[0]][map_dimensions[1]])
{
	extern int map_dimensions[2];
	// generate map
	// map is a array of arrays:
	// map[map_dimensions[0]][map_dimensions[1]]
	//
	// fields are accesed like map[x][y] (which could be '.' or '#' etc.)
	
	int mapx;
	for(mapx = 0; mapx < map_dimensions[0]; mapx++)
	{
		int mapy;
		for(mapy = 0; mapy < map_dimensions[1]; mapy++){
			map[mapx][mapy] = '.';
		}

	}
}

void draw(int map[map_dimensions[0]][map_dimensions[1]], int player[3], int level_exit[2], int monsterc, int monsters[][2])
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
	
	tb_change_cell(tb_width() - 1,0,player[2] + 0x30,TB_WHITE,TB_DEFAULT);	
	for(int i = 0; i < monsterc; i++)
	{
		// the monster hasn't been put on the "graveyard" in (-1,-1)
		if(monsters[i][0] != - 1 && monsters[i][1] != - 1) 
		{
			tb_change_cell(monsters[i][0],monsters[i][1], 'm', TB_RED, TB_DEFAULT);
		}
	}
	
	tb_change_cell(player[0], player[1], '@', TB_MAGENTA, TB_DEFAULT);
	tb_change_cell(level_exit[0], level_exit[1], 0x2588,TB_WHITE,TB_DEFAULT);

	
	tb_present();
}

int test_position(int x, int y)
{
	// is the position in the terminal?
	if(x >= 0 && x < tb_width() && y >= 0 && y < tb_height()){ 
		return 1;
	}else{
		return 0;
	}
}

// returns - 1 if false; otherwise the index of the monster in monsters
int test_for_monsters(int x, int y, int monsterc, int monsters[][2]) 
{
	// walk trough the monsters and check if one is at the specific point
	for(int i = 0; i < monsterc; i++)
	{
		if(monsters[i][0] == x && monsters[i][1] == y)
		{
			return i;
		}
	}
	return - 1;
}

void fight(int player[3], int monsteri, int monsters[][2])
{
	player[2] = player[2] - 1;
	monsters[monsteri][0] = -1; // put the monster into the "graveyard"
	monsters[monsteri][1] = -1; // monsters at (-1,-1) are simply ignored 8)
}

void handle_move(int new_x, int new_y, int player[3], int monsterc, int monsters[][2])
{
	int monster_there = test_for_monsters(new_x, new_y, monsterc, monsters);

	// position is in the terminal and there's no monster -> move there
	if(test_position(new_x, new_y) == 1 && monster_there == - 1)
	{ 
		player[0] = new_x;
		player[1] = new_y;
	}
	// there's a monster -> fight
	else if(test_position(new_x, new_y) == 1 && monster_there != - 1) 
	{
		fight(player, monster_there, monsters); 
	}
}

void move(int player[], uint16_t key, uint32_t ch, int monsterc, int monsters[][2])
{
	int new_x, new_y;
	
	if(key == TB_KEY_ARROW_UP || ch == 'k')
	{
		new_x = player[0];
		new_y = player[1] - 1;

		handle_move(new_x, new_y, player, monsterc, monsters);
	}
	else if(key == TB_KEY_ARROW_DOWN || ch == 'j')
	{
		new_x = player[0];
		new_y = player[1] + 1;

		handle_move(new_x, new_y, player, monsterc, monsters);	
	}
	else if(key == TB_KEY_ARROW_LEFT || ch == 'h')
	{
		new_x = player[0] - 1;
		new_y = player[1];
		
		handle_move(new_x, new_y, player, monsterc, monsters);		
	}
	else if(key == TB_KEY_ARROW_RIGHT || ch == 'l')
	{
		new_x = player[0] + 1;
		new_y = player[1];
		
		handle_move(new_x, new_y, player, monsterc, monsters);
	}
}

void move_monsters(int player[3], int monsterc, int monsters[][2])
{
	for(int i = 0; i < monsterc; i++)
	{
		// is the monster on the "graveyard" at (-1,-1)
		if(monsters[i][0] == -1 || monsters[i][1] == -1)
		{
		     continue;
		}
		// calculate x-distance and y-distance
		int xdist = monsters[i][0] - player[0];
		int ydist = monsters[i][1] - player[1];

		// is there no way to go?
		int nulldist = (ydist == 0) && (xdist == 0); 
		
		if(ydist > 0 && ydist >= xdist && !nulldist)
		{
			monsters[i][1] = monsters[i][1] - 1;
		}
		else if(ydist < 0 && ydist < xdist && !nulldist)
		{
			monsters[i][1] = monsters[i][1] + 1;
		}
		else if(xdist > 0 && xdist >= ydist && !nulldist)
		{
			monsters[i][0] = monsters[i][0] - 1;
		}
		else if(xdist < 0 && xdist < ydist && !nulldist)
		{
			monsters[i][0] = monsters[i][0] + 1;
		}

		if(monsters[i][0] == player[0] && monsters[i][1] == player[1])
		{
			fight(player, i, monsters);
		}
	}	
}

int main(int argc, char *argv[])
{
	extern int map_dimensions[2];

	int player[3] = { 0, 0, 5 }; // { x-position of the @, y-position of the @, lives of @ }
	int level_exit[2]; // { x-pos of exit, y-pos of exit }
	struct tb_event event; // here will our events be stored

	tb_init();

	// pick a random location for the exit
	srand(time(NULL));	
	level_exit[0] = rand() % tb_width();
	level_exit[1] = rand() % tb_height();

	// init the size of the map. The map does NOT resize
	map_dimensions[0] = tb_width();
	map_dimensions[1] = tb_height();
	
	// create an array of the size
	// see generate_map for a further explanition
	// of the data structure
	int map[map_dimensions[0]][map_dimensions[1]];
	
	generate_map(map);

	int monsterc = rand() % 10;
	int monsters[monsterc][2];
	
	for(int i = 0; i < monsterc; i++)
	{
		monsters[i][0] = rand() % tb_width();
		monsters[i][1] = rand() % tb_height();
	}

	while(1)
	{
		draw(map, player, level_exit, monsterc, monsters);
		tb_poll_event(&event); // wait for an event
		//debug("got event");
		switch(event.type)
		{
			case TB_EVENT_KEY: // a key got pressed
				switch(event.key)
				{
					case TB_KEY_ESC: // we need to exit
						tb_shutdown();
						exit(0);
						break; // yolo
					default: // let the move-function check if we have to move or not
						move(player,event.key, event.ch, monsterc, monsters);
						break;
				}
				if(event.ch == 'q')
				{
					tb_shutdown();
					exit(0);
				}
				break;
		}
		
		/* Did we get to the exit? */
		if(player[0] == level_exit[0] && player[1] == level_exit[1])
		{
			tb_shutdown();
			printf("\\o/ You escaped! \\o/\n");
			break;	
		}	

		move_monsters(player, monsterc, monsters);

		/* are we dead? */
		if(player[2] == 0){
			tb_shutdown();
			printf("/o\\ You are dead /o\\\n");
			break;
		}
	}

	return 0;
}
