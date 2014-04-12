#include "../termbox/termbox.h"
//#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define COUNT_OF( arr) (sizeof(arr)/sizeof(0[arr]))

void draw(int player[2], int level_exit[2], int monsterc, int monsters[][2])
{
	tb_clear();
	
	for(int i = 0; i < monsterc; i++)
	{
		tb_change_cell(monsters[i][0],monsters[i][1], 'm', TB_RED, TB_DEFAULT);
	}
	
	tb_change_cell(player[0], player[1], '@', TB_MAGENTA, TB_DEFAULT);
	tb_change_cell(level_exit[0], level_exit[1], '>',TB_WHITE,TB_DEFAULT);

	
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

int test_for_monsters(int x, int y, int monsterc, int monsters[][2])
{
	for(int i = 0; i < monsterc; i++) // walk trough the monsters and check if one is at the specific point
	{
		if(monsters[i][0] == x && monsters[i][1] == y){
			return 1;
		}
	}
	return 0;
}

void handle_move(int new_x, int new_y, int player[2], int monsterc, int monsters[][2])
{
	int monster_there = test_for_monsters(new_x, new_y, monsterc, monsters);

	if(test_position(new_x, new_y) == 1 && monster_there == 0){ // position is in the terminal and there's no monster -> move there
		player[0] = new_x;
		player[1] = new_y;
	}
	else if(test_position(new_x, new_y) == 1 && monster_there == 1) // there's a monster -> fight
	{
		// fight 
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
		default:
			break;
	}
	/*debug("new_x: %d", new_x);
	debug("new_y: %d", new_y);
	debug("player[0]: %d",player[0]);
	debug("player[1]: %d",player[1]);*/
}


int main(int argc, char *argv[])
{
	int player[2] = { 0, 0 }; // { x-position of the @, y-position of the @ }
	int player_lives = 5;
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
			default:
				break;
		}

		/* Did we get to the exit? */
		if(player[0] == level_exit[0] && player[1] == level_exit[1])
		{
			tb_shutdown();
			printf("\\o/ You escaped! \\o/\n");
			exit(0);
		}	
	}

	return 0;
}
