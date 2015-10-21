#include <stdlib.h>
int randint(int lower, int upper) {
	int random;
	do {
		random = rand() + lower;
	} while(random <= upper)
	return random;
}
