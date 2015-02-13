#include "util.h"
#include "data.h"
#include "config.h"

unsigned int count_surrounding_walls(unsigned int x, unsigned int y, struct World *world) {
	unsigned int count = 0;
	int nx, ny;

	for(nx = -1; nx <= 1; nx++) {
		if((int) x + nx < MAP_START_X || x + nx >= MAP_END_X) {
			count++;
			continue;
		}

		for(ny = -1; ny <= 1; ny++) {
			if((int) y + ny < MAP_START_Y || y + ny >= MAP_END_Y) {
				count++;
				continue;
			}

			if(world->map[x + nx][y + ny] == '#') {
				count++;
			}
		}
	}
	return count;
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

	// TODO: Document the generation process clearly
	// The basic Idea is this one: http://www.roguebasin.com/index.php?title=Cellular_Automata_Method_for_Generating_Random_Cave-Like_Levels
	// randomly add walls
	for(mapx = MAP_START_X; mapx < MAP_END_X; mapx++) {
		for(mapy = MAP_START_Y; mapy < MAP_END_Y; mapy++) {
			// wall propability = 45%
			if(randint(0,100) < 45) {
				world->map[mapx][mapy] = WALL;
			}
		}
	}

	// use our cellular automatons to make something reasonable out of it
	//int cai;
	int i, repeatn = randint(1, 3);

	for(i = 0; i < repeatn; i++) {
		for(mapx = MAP_START_X; mapx < MAP_END_X; mapx++) {
			for(mapy = MAP_START_Y; mapy < MAP_END_Y; mapy++) {
				unsigned int surrounding_walls = count_surrounding_walls(mapx, mapy, world);
				if(surrounding_walls >= 5) {
					world->map[mapx][mapy] = WALL;
				} else {
					world->map[mapx][mapy] = GROUND;
				}
			}
		}
	}
}
