#include <stdlib.h>
int randint(int lower, int upper) {
	return rand() % upper + lower;
}
