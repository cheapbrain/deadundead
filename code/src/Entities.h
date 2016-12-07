#pragma once
enum Tag
{
	UPDATE = 1<<0,
	RENDER = 1<<1,
	COLLIDE = 1<<2,
	HITTABLE = 1<<3
};


struct Entity {
	int id, tag;
	float x, y, width, height;
	float speed_x, speed_y;
	char *name;
	float health;

	void *update(const void*, float);
	void *render(const void*, float);
	int *colliding(const void*, const void*);
	void *on_collide();
	void *on_hit();
	void *on_enter();
	void *on_interact();
};
