#include "../termbox/termbox.h"
//#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define COUNT_OF( arr) (sizeof(arr)/sizeof(0[arr]))

void draw(int player[3], int level_exit[2], int monsterc, int monsters[][2])
{
	tb_clear();
	
	tb_change_cell(tb_width() - 1,0,player[2] + 0x30,TB_WHITE,TB_DEFAULT);	
	for(int i = 0; i < monsterc; i++)
	{
		// the monster hasn't been put on the "graveyard" in (-1,-1)
		if(monsters[i][0] != - 1 && monsters[i][1] != - 1) 		{
			tb_change_cell(monsters[i][0],monsters[i][1], 'm', TB_RED, TB_DEFAULT);
		}
	}
	
	tb_change_cell(player[0], player[1], '@', TB_MAGENTA, TB_DEFAULT);
	tb_change_cell(level_exit[0], level_exit[1], 0x2588,TB_WHITE,TB_DEFAULT);

	
	tb_present();
}

int test_position(int x, int y)
{
	if(x >= 0 && x < tb_width() && y >= 0 && y < tb_height()){ // is the position in the terminal?
		return 1;
	}else{
		return 0;
	}
}

int test_for_monsters(int x, int y, int monsterc, int monsters[][2]) // returns - 1 if false; otherwise the index of the monster in monsters
{
	for(int i = 0; i < monsterc; i++) // walk trough the monsters and check if one is at the specific point
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

	if(test_position(new_x, new_y) == 1 && monster_there == - 1)
	{ // position is in the terminal and there's no monster -> move there
		player[0] = new_x;
		player[1] = new_y;
	}
	else if(test_position(new_x, new_y) == 1 && monster_there != - 1) // there's a monster -> fight
	{
		fight(player, monster_there, monsters); 
	}
}

void move(int player[], uint16_t key, int monsterc, int monsters[][2])
{
	int new_x, new_y;
	switch(key)
	{
		case TB_KEY_ARROW_UP:
			new_x = player[0];
			new_y = player[1] - 1;

			handle_move(new_x, new_y, player, monsterc, monsters);
			
			break;
		case TB_KEY_ARROW_DOWN:
			new_x = player[0];
			new_y = player[1] + 1;

			handle_move(new_x, new_y, player, monsterc, monsters);	

			break;
		case TB_KEY_ARROW_LEFT:
			new_x = player[0] - 1;
			new_y = player[1];
			
			handle_move(new_x, new_y, player, monsterc, monsters);
			
			break;
		case TB_KEY_ARROW_RIGHT:
			new_x = player[0] + 1;
			new_y = player[1];
			
			handle_move(new_x, new_y, player, monsterc, monsters);
			
			break;
	}
	/*debug("new_x: %d", new_x);
	debug("new_y: %d", new_y);
	debug("player[0]: %d",player[0]);
	debug("player[1]: %d",player[1]);*/
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
		int xdist = monsters[i][0] - player[0]; // x distance
		int ydist = monsters[i][1] - player[1]; // y distance

		// is there no way to go?
		int nulldist = (ydist == 0) && (xdist == 0); 
		/*if(xdist == 0 && ydist == 0)
		{
			// nothing to do here. You'll die little monster :(	
		}*/ 
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
	int player[3] = { 0, 0, 5 }; // { x-position of the @, y-position of the @, lives of @ }
	int level_exit[2]; // { x-pos of exit, y-pos of exit }
	struct tb_event event; // here will our events be stored

	tb_init();

	/* pick a random location for the exit*/
	srand(time(NULL));	
	level_exit[0] = rand() % tb_width();
	level_exit[1] = rand() % tb_height();

	int monsterc = rand() % 10;
	int monsters[monsterc][2];
	
	for(int i = 0; i < monsterc; i++)
	{
		monsters[i][0] = rand() % tb_width();
		monsters[i][1] = rand() % tb_height();
	}

	while(1)
	{
		draw(player, level_exit, monsterc, monsters);
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
						move(player,event.key, monsterc, monsters);
						break;
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
