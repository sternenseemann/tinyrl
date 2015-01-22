void exit_gracefully(int code, struct World *world);

void ensure(int condition, struct World *world);

unsigned int **allocate_map(struct World *world);

void free_map(struct World *world);
