#include "physics.h"
#include <stdio.h>
#include <float.h>

int collides(struct Rectangle *r1, struct Rectangle *r2) {
	return	(r2->pos.x > r1->pos.x - r2->size.x) && (r2->pos.x < r1->pos.x + r1->size.x) && 
			(r2->pos.y > r1->pos.y - r2->size.y) && (r2->pos.y < r1->pos.y + r1->size.y);
}

int collides(Vec2 *center, float radius, Rectangle *r) {
	Vec2 r_center;
	r_center.x = r->pos.x + (r->size.x)/2;
	r_center.y = r->pos.y + (r->size.y)/2;
	return distance2(center, &r_center) <= (radius*radius);
}

float distance2(Vec2 *pos1, Vec2 *pos2) {
	return (pos1->x - pos2->x)*(pos1->x - pos2->x) + (pos1->y - pos2->y)*(pos1->y - pos2->y);
}

Entity *closest(Vec2 *pos, float max_dist, World *world, EntityListType list) {
	EntityList *el = &(world->lists[list]);
	float min_dist = FLT_MAX;
	float curr_dist;
	Entity *e_res = NULL;
	Entity *e_curr;
	Vec2 e_center;
	for (int i = 0; i < el->count; i++) {
		e_curr = world_get_entity(world, el->entity_id[i]);
		e_center = get_entity_center(e_curr);
		curr_dist = distance2(&e_center, pos);
		if (curr_dist < min_dist && curr_dist < max_dist*max_dist) {
			min_dist = curr_dist;
			e_res = e_curr;
		}
	}
	return e_res;
}

Vec2 get_entity_center(Entity *e) {
	Vec2 res = {e->x+e->width/2, e->y+e->height/2};
	return res;
}

Rectangle get_entity_rectangle(Entity *e) {
	Rectangle res = {{e->x, e->y}, {e->width, e->height}};
	return res;
}