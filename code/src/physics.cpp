#include "physics.h"

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