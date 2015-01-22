#include <stdlib.h>
#include "data.h"
#include "allocation.h"

void exit_gracefully(int code, struct World *world) {
	if(world != NULL) {
		free_map(world);

		if(world->monsters != NULL) {
			free(world->monsters);
		}

		free(world);
	}

	exit(code);
}

/* If condition is false, it causes a graceful exit,
 * otherwise just does nothing */
void ensure(int condition, struct World *world) {
	if(!condition) {
		exit_gracefully(EXIT_FAILURE, world);
	}
}
unsigned int **allocate_map(struct World *world) {
	unsigned int ** map = malloc(world->map_dimensions[0] * sizeof(unsigned int *));
	unsigned int i;
	for (i = 0; i < world->map_dimensions[0]; i++) {
		map[i] = malloc(world->map_dimensions[1] * sizeof(unsigned int));
		ensure(map[i] != NULL, world);
	}

	return map;
}

void free_map(struct World *world) {
	if(world->map == NULL) {
		return;
	}

	unsigned int i;
	for(i = 0; i < world->map_dimensions[0]; i++) {
		if(world->map[i] != NULL) {
			free(world->map[i]);
		}
	}

	free(world->map);
}


/* Broken code which fixed version could lead to performance gain

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


