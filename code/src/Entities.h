#pragma once

#include "utils.h"

typedef struct World World;
typedef struct Entity Entity;

enum EntityTag
{
	UPDATE = 1<<0,
	RENDER = 1<<1,
	RENDER_BACK = 1<<2,
	RENDER_FRONT = 1<<3,
	DYNAMIC_COLLIDE = 1<<4,
	STATIC_COLLIDE = 1<<5,
	HITTABLE = 1<<6,
	ACTIVE_EVENT = 1<<7,
	PASSIVE_EVENT = 1<<8
};

enum EntityListType
{
	UPDATE_LIST = 0,
	RENDER_LIST,
	RENDER_BACK_LIST,
	RENDER_FRONT_LIST,
	DYNAMIC_COLLIDE_LIST,
	STATIC_COLLIDE_LIST,
	HITTABLE_LIST,
	ACTIVE_EVENT_LIST,
	PASSIVE_EVENT_LIST,
	_LIST_COUNT
};

const EntityTag list_tags[_LIST_COUNT] = {
	UPDATE,
	RENDER,
	RENDER_BACK,
	RENDER_FRONT,
	DYNAMIC_COLLIDE,
	STATIC_COLLIDE,
	HITTABLE,
	ACTIVE_EVENT,
	PASSIVE_EVENT
};

struct EntityList {
	int id;
	int count;
	int capacity;
	int *entity_id;
};

struct Entity {
	int id;
	int tag;
	float x, y, width, height;
	float old_x, old_y;
	float speed_x, speed_y;
	float health;
	Texture *texture;
	int is_on_floor;
	int player_id;
	float bounce_coeff;

	void (*update)(Entity *e, World *world, double delta);
	void (*render)(Entity *e);
	int (*colliding)(const void*, const void*);
	void (*on_collide)();
	void (*on_hit)();
	void (*on_enter)();
	void (*on_interact)();

	int indexes[_LIST_COUNT];
};

struct World {
	Vec2 size;
	Entity *entities;
	int entity_count;
	int entity_capacity;
	
	EntityList lists[_LIST_COUNT];
};

void world_init(World *world, int capacity);

void world_render(World *world, double delta);

void world_update(World *world, double delta);

Entity *world_new_entity(World *world, int tag);

void world_remove_entity(World *world, int id);

Entity *world_get_entity(World *world, int id);

void world_resize_entity_list(World *world, int new_capacity);

void entity_remove_list(World *world, Entity *entity, EntityListType list_id);

void entity_add_list(World *world, Entity *entity, EntityListType list_id);