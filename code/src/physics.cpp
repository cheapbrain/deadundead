#include "utils.h"
#include "physics.h"
#include <stdlib.h>

#define NUM_BYTES_AREA 4000
#define ENTITY sostituireconstructentity

int collides(struct Rectangle *r1, struct Rectangle *r2) {
	return	(r2->pos->x > r1->pos->x - r2->size->x) && (r2->pos->x < r1->pos->x + r1->size->x) && 
			(r2->pos->y > r1->pos->y - r2->size->y) && (r2->pos->y < r1->pos->y + r1->size->y);
}

/*
char *search_in_area(char *list, int size, int entity_size, struct Rectangle *area) {
	static char res[NUM_BYTES_AREA];
	int list_curr, res_curr = 0;

	for (list_curr = 0; list_curr < size; list_curr++) {
		ENTITY curr = (ENTITY)(char[list_curr*entity_size]);
		struct Rectangle r = {{curr.x, curr.y}, {curr.size_x, curr.size_y}};

		if (collides(&r, area)) {
			//TODO funziona?
			res[res_curr*entity_size] = 
		}
	}

	return &res;
}
*/
ENTITY **search_in_area(ENTITY **list, int size, struct Rectangle *area) {
	static ENTITY *res[NUM_BYTES_AREA];
	int list_curr, res_curr = 0;

	for (list_curr = 0; list_curr < size; list_curr++) {
		ENTITY *curr = list[list_curr];
		struct Rectangle r = {{curr->x, curr->y}, {curr->size_x, curr->size_y}};

		if (collides(&r, area)) {
			res[res_curr] = curr;
			res_curr++;
		}
	}

	res[res_curr] = NULL;
	return &res;
}