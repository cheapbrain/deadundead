#include <stdlib.h>

#include "entities.h"
#include "screen.h"
#include "utils.h"
#include "renderer.h"
#include "physics.h"


int _list_add(EntityList *list, World *world, Entity *entity) {
	if (list->count == list->capacity) {
		list->capacity *= 2;
		list->entity_id = (int *)realloc(list->entity_id, sizeof (int) * list->capacity);
	}
	int index = list->count++;
	list->entity_id[index] = entity->id;
	return index;
}

void _list_remove(EntityList *list, World *world, int index) {
	list->count--;
	if (index < list->count) {
		list->entity_id[index] = list->entity_id[list->count];
		world_get_entity(world, list->entity_id[index])->indexes[list->id] = index;
	}
}

void world_init(World *world, int capacity) {
	world->size.x = 17;
	world->size.y = 10;
	world->transform = (Mat3 *)malloc(sizeof(Mat3));
	orthographic(world->transform, 10, 0, 0);
	world->entity_count = 0;
	world->entity_capacity = capacity;
	world->entities = (Entity *)malloc(world->entity_capacity * sizeof Entity);

	for (int i = 0; i < _LIST_COUNT; i++) {
		EntityList *list = &world->lists[i];
		list->id = i;
		list->count = 0;
		list->capacity = 100;
		list->entity_id = (int *)malloc(sizeof (int) * 100);
	}
}


void world_update(World *world, double delta) {
	EntityList *update_list = &world->lists[UPDATE_LIST];
	for (int i = 0; i < update_list->count; i++) {
		Entity *e = world_get_entity(world, update_list->entity_id[i]);
		e->update(e, world, delta);
	}

	EntityList *dynamic_list = &world->lists[DYNAMIC_COLLIDE_LIST];
	EntityList *static_list = &world->lists[STATIC_COLLIDE_LIST];

	for (int i = 0; i < static_list->count; i++) {
		Entity *st = world_get_entity(world, static_list->entity_id[i]);
		Rectangle rect_static = {{st->x, st->y},{st->width, st->height}};
		for (int j = 0; j < dynamic_list->count; j++) {
			Entity *dn = world_get_entity(world, dynamic_list->entity_id[j]);
			Rectangle rect_dyn = {{dn->x, dn->old_y},{dn->width, dn->height}};
			if (dn->x - dn->old_x != 0 && collides(&rect_static, &rect_dyn)) {
				float overlap;
				if (dn->x > dn->old_x) overlap = dn->x + dn->width - st->x + .001f;
				else overlap = dn->x - st->x - st->width - .001f;
				rect_dyn.pos.x -= overlap;
				dn->x -= overlap;
				dn->speed_x = 0;
			}
			rect_dyn.pos.y = dn->y;
			rect_dyn.pos.x = dn->old_x;
			if (dn->y - dn->old_y != 0) {
				if (collides(&rect_static, &rect_dyn)) {
					float overlap;
					if (dn->y > dn->old_y) overlap = dn->y + dn->height - st->y + .001f;
					else {
						overlap = dn->y - st->y - st->height - .001f;
						dn->is_on_floor = 1;
					}
					dn->y -= overlap;
					dn->speed_y = 0;
				} else dn->is_on_floor = 0;
			}
		}
	}
}

void world_render(World *world, double delta) {
	set_matrix(&game.renderer, world->transform);

	EntityList *back_layer = &world->lists[RENDER_BACK_LIST];
	EntityList *middle_layer = &world->lists[RENDER_LIST];
	EntityList *front_layer = &world->lists[RENDER_FRONT_LIST];

	for (int i = 0; i < back_layer->count; i++) {
		Entity *e = world_get_entity(world, back_layer->entity_id[i]);
		e->render(e);
	}
	for (int i = 0; i < middle_layer->count; i++) {
		Entity *e = world_get_entity(world, middle_layer->entity_id[i]);
		e->render(e);
	}
	for (int i = 0; i < front_layer->count; i++) {
		Entity *e = world_get_entity(world, front_layer->entity_id[i]);
		e->render(e);
	}
}

Entity *world_new_entity(World *world, int tag) {
	if (world->entity_count == world->entity_capacity) {
		world->entity_capacity *= 2;
		world->entities = (Entity *)realloc(world->entities, sizeof Entity * world->entity_capacity);
	}
	Entity *new_entity = world->entities + world->entity_count;
	new_entity->id = world->entity_count++;
	new_entity->tag = tag;

	for (int i = 0; i < _LIST_COUNT; i++) {
		if (list_tags[i] & tag) {
			new_entity->indexes[i] = _list_add(&world->lists[i], world, new_entity);
		} else {
			new_entity->indexes[i] = -1;
		}
	}
	return new_entity;
}

void world_remove_entity(World *world, int id) {
	Entity *entity = &world->entities[id];
	for (int i = 0; i < _LIST_COUNT; i++) {
		if (entity->indexes[i] >= 0) {
			_list_remove(&world->lists[i], world, entity->id);
		}
	}

	world->entity_count--;
	if (id < world->entity_count) {
		world->entities[id] = world->entities[world->entity_count];
		for (int i = 0; i < _LIST_COUNT; i++) {
			if (entity->indexes[i] >= 0) {
				world->lists[i].entity_id[entity->indexes[i]] = entity->id;
			}
		}
	}
}

Entity *world_get_entity(World *world, int id) {
	Entity *e = world->entities + id;
	return e;
}

void world_resize_entity_list(World *world, int new_capacity) {
	if (new_capacity < world->entity_count) new_capacity = world->entity_count;
	world->entities = (Entity *)realloc(world->entities, sizeof Entity * world->entity_capacity);
	world->entity_capacity = new_capacity;
}

void entity_remove_list(World *world, Entity *entity, EntityListType list_id) {
	if (entity->indexes[list_id] >= 0) {
		entity->tag = entity->tag & ~list_tags[list_id];
		_list_remove(&world->lists[list_id], world, entity->indexes[list_id]);
		entity->indexes[list_id] = -1;
	}
}

void entity_add_list(World *world, Entity *entity, EntityListType list_id) {
	if (entity->indexes[list_id] < 0) {
		entity->indexes[list_id] = _list_add(&world->lists[list_id], world, entity);
		entity->tag = entity->tag | list_tags[list_id];
	}
}