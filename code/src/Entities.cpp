#include <stdlib.h>

#include "entities.h"
#include "screen.h"
#include "utils.h"
#include "renderer.h"
#include "physics.h"
#include "gameplay.h"
#include "objects_data.h"

int _list_add(EntityList *list, World *world, Entity *entity) {
	if (list->count == list->capacity) {
		list->capacity *= 2;
		list->entity_indexes = (int *)realloc(list->entity_indexes, sizeof (int) * list->capacity);
	}
	int index = list->count++;
	list->entity_indexes[index] = entity->index;
	return index;
}

void _list_remove(EntityList *list, World *world, int index) {
	list->count--;
	if (index < list->count) {
		list->entity_indexes[index] = list->entity_indexes[list->count];
		(world->entities + list->entity_indexes[index])->indexes[list->id] = index;
	}
}

void world_init(World *world, int capacity) {
	world->size.x = 17;
	world->size.y = 10;
	world->entity_count = 0;
	world->entity_capacity = capacity;
	world->entities = (Entity *)malloc(world->entity_capacity * sizeof Entity);
	world->free_id = 1;

	int_set_init(&(world->to_be_removed), 20);

	for (int i = 0; i < _LIST_COUNT; i++) {
		EntityList *list = &world->lists[i];
		list->id = i;
		list->count = 0;
		list->capacity = 100;
		list->entity_indexes = (int *)malloc(sizeof (int) * 100);
	}
}

void world_actual_remove_entity(World *world, int id);
void world_update(World *world, double delta) {
	EntityList *update_list = &world->lists[UPDATE_LIST];
	for (int i = 0; i < update_list->count; i++) {
		Entity *e = world->entities + update_list->entity_indexes[i];
		e->update(e, world, delta);
	}

	EntityList *dynamic_list = &world->lists[DYNAMIC_COLLIDE_LIST];
	EntityList *static_list = &world->lists[STATIC_COLLIDE_LIST];
	//salvo gli id delle entità dinamiche che hanno fatto collisione per fargli eseguire la funzione dopo e solo una volta (se la eseguissi sul momento potre aver problemi se la funzione chiede per esempio una eliminazione)
	IntSet collided_set;
	int_set_init(&collided_set, static_list -> count);
	//salvo anche la prima entità statica con cui hanno fatto collisione
	ArrayList collided_with;
	list_init(&collided_with, sizeof(Entity *), static_list -> count);
	
	//risolvo collisioni x e y tra dynamic e terrain
	/*BUGGO
	for (int i = 0; i < static_list->count; i++) {
		Entity *st = world_get_entity(world, static_list->entity_id[i]);
		Rectangle rect_static = {{st->x, st->y},{st->width, st->height}};
		for (int j = 0; j < dynamic_list->count; j++) {
			int has_collided = 0;
			Entity *dn = world_get_entity(world, dynamic_list->entity_id[j]);
			Rectangle rect_dyn = {{dn->x, dn->old_y},{dn->width, dn->height}};
			if (dn->x != dn->old_x && collides(&rect_static, &rect_dyn)) {
				float overlap;
				if (dn->x > dn->old_x) overlap = dn->x + dn->width - st->x + .0001f;
				else overlap = dn->x - st->x - st->width - .0001f;
				dn->x -= overlap;
				dn->speed_x = 0;
				has_collided = 1;
			}
			rect_dyn.pos.y = dn->y;
			if (dn->y != dn->old_y && collides(&rect_static, &rect_dyn)) {
				float overlap;
				if (dn->y > dn->old_y) overlap = dn->y + dn->height - st->y + .0001f;
				else {
					overlap = dn->y - st->y - st->height - .0001f;
					dn->is_on_floor = 1;
				}
				dn->y -= overlap;
				dn->speed_y = -dn->speed_y * (st->bounce_coeff + dn->bounce_coeff)/2;
				has_collided = 1;
			}

			if (has_collided) {
				if (int_set_add(&collided_set, dynamic_list->entity_id[j])) {
					list_set(&collided_with, collided_set.count - 1, (void *)st);
				}
			}
		}
	}
	*/

	//risolvo collisioni sulla x dei giocatori con il terrain
	for (int i = 0; i < static_list->count; i++) {
		Entity *st = world->entities + static_list->entity_indexes[i];
		Rectangle rect_static = {{st->x, st->y},{st->width, st->height}};
		for (int j = 0; j < dynamic_list->count; j++) {
			Entity *dn = world->entities + dynamic_list->entity_indexes[j];
			Rectangle rect_dyn = {{dn->x, dn->old_y},{dn->width, dn->height}};
			if (dn->x != dn->old_x && collides(&rect_static, &rect_dyn)) {
				float overlap;
				if (dn->x > dn->old_x) overlap = dn->x + dn->width - st->x + .0001f;
				else overlap = dn->x - st->x - st->width - .0001f;
				dn->x -= overlap;
				//dn->speed_x = 0;
				dn->speed_x = -dn->speed_x * (dn->bounce_coeff + st->bounce_coeff)/2;
				if (int_set_add(&collided_set, dynamic_list->entity_indexes[j])) {
					list_set(&collided_with, collided_set.count - 1, (void *)st);
				}
			}
		}
	}

	//risolvo collisioni sulla y dei giocatori con il terrain
	for (int i = 0; i < static_list->count; i++) {
		Entity *st = world->entities + static_list->entity_indexes[i];
		Rectangle rect_static = {{st->x, st->y},{st->width, st->height}};
		for (int j = 0; j < dynamic_list->count; j++) {
			Entity *dn = world->entities + dynamic_list->entity_indexes[j];
			Rectangle rect_dyn = {{dn->x, dn->y},{dn->width, dn->height}};
			if (dn->y != dn->old_y) {
				if (collides(&rect_static, &rect_dyn)) {
					float overlap;
					if (dn->y > dn->old_y) overlap = dn->y + dn->height - st->y + .0001f;
					else {
						overlap = dn->y - st->y - st->height - .0001f;
						dn->is_on_floor = 1;
					}
					dn->y -= overlap;
					//dn->speed_y = -dn->speed_y * st->bounce_coeff;
					dn->speed_y = -dn->speed_y * (st->bounce_coeff + dn->bounce_coeff)/2;
					if (int_set_add(&collided_set, dynamic_list->entity_indexes[j])) {
						list_set(&collided_with, collided_set.count - 1, (void *)st);
					}
				}
			}
		}
	}
	
	//le faccio collidere
	for (int i = 0; i < collided_set.count; i++) {
		Entity *e = world->entities + collided_set.elements[i];
		if (e -> on_collide != NULL) {
			//Rectangle hitbox = {{e->x, e->y},{0.5f,0.5f}};
			e -> on_collide(e, (Entity *) list_get(&collided_with, i), world);
			//test_hitbox(world, &hitbox);
		}
	}
	int_set_destroy(&collided_set);
	free(collided_with.array);
	
	//elimino le entità in attesa di rimozione
	for (int i = 0; i < world->to_be_removed.count; i++) {
		world_actual_remove_entity(world, world->to_be_removed.elements[i]);
	}
	world->to_be_removed.count = 0;
}

void world_render(World *world, double delta) {

	EntityList *back_layer = &world->lists[RENDER_BACK_LIST];
	EntityList *middle_layer = &world->lists[RENDER_LIST];
	EntityList *front_layer = &world->lists[RENDER_FRONT_LIST];

	for (int i = 0; i < back_layer->count; i++) {
		Entity *e = world->entities + back_layer->entity_indexes[i];
		e->render(e);
	}
	for (int i = 0; i < middle_layer->count; i++) {
		Entity *e = world->entities + middle_layer->entity_indexes[i];
		e->render(e);
	}
	for (int i = 0; i < front_layer->count; i++) {
		Entity *e = world->entities + front_layer->entity_indexes[i];
		e->render(e);
	}
}

Entity *world_new_entity(World *world, int tag) {
	if (world->entity_count == world->entity_capacity) {
		world->entity_capacity *= 2;
		world->entities = (Entity *)realloc(world->entities, sizeof Entity * world->entity_capacity);
	}
	Entity *new_entity = world->entities + world->entity_count;
	new_entity->id = world->free_id++;
	new_entity->index = world->entity_count++;
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

Entity *world_new_entity(World *world, Entity *entity) {
	int tag = entity->tag;
	if (world->entity_count == world->entity_capacity) {
		world->entity_capacity *= 2;
		world->entities = (Entity *)realloc(world->entities, sizeof Entity * world->entity_capacity);
	}
	Entity *new_entity = world->entities + world->entity_count;
	*new_entity = *entity;
	new_entity->id = world->free_id++;
	new_entity->index = world->entity_count++;
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

void world_remove_entity(World *world, int index) {
	int_set_add(&(world->to_be_removed), index);
}

static void world_actual_remove_entity(World *world, int index) {
	Entity *entity = world->entities + index;
	for (int i = 0; i < _LIST_COUNT; i++) {
		if (entity->indexes[i] >= 0) {
			_list_remove(&world->lists[i], world, entity->indexes[i]);
		}
	}

	world->entity_count--;
	if (index < world->entity_count) {
		*entity = world->entities[world->entity_count];
		entity->index = index;
		for (int i = 0; i < _LIST_COUNT; i++) {
			if (entity->indexes[i] >= 0) {
				world->lists[i].entity_indexes[entity->indexes[i]] = index;
			}
		}
	}
}

Entity *world_get_entity(World *world, int id) {
	Entity *e = world->entities;
	Entity *last = e + world->entity_count;
	while (e < last) {
		if (e->id == id) return e;
		else e++;
	}
	return NULL;
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

void player_update(Entity *e, World *world, double delta) {
	e->old_x = e->x;
	e->old_y = e->y;

	if (button_pressed(B_EMOTE2, e->player_id)) {
		e->animation.character = load_spriter_character("../anim_export/Diogo.scon");
	}

	if (button_pressed(B_EMOTE3, e->player_id)) {
		e->animation.character = load_spriter_character("../anim_export/Frollo.scon");
	}

	if (button_pressed(B_EMOTE4, e->player_id)) {
		e->animation.character = load_spriter_character("../anim_export/Klora.scon");
	}

	if (e->is_on_floor) {
		if (e->speed_x > 0.1f || e->speed_x < -0.1f) {
			play_animation(&e->animation, ANIMATION_STATE_RUN, ANIMATION_WEAPON_HANDS);
		} else
			play_animation(&e->animation, ANIMATION_STATE_IDLE, ANIMATION_WEAPON_HANDS);
	} else {
		play_animation(&e->animation, ANIMATION_STATE_IN_AIR, ANIMATION_WEAPON_HANDS);
	}

	//e->speed_x = 0;
	e->speed_x -= 10 * e->speed_x * (float)delta;
	if (button_state(B_RIGHT, e->player_id)) {e->speed_x += 40.f * (float)delta; e->is_facing_right = 1;}
	if (button_state(B_LEFT, e->player_id)) {e->speed_x -= 40.f * (float)delta; e->is_facing_right = 0;}

	e->speed_y -= (float)(17.f * delta);

	if (e->is_on_floor && button_state(B_JUMP, e->player_id)) {
		e->speed_y += 8.5f;
	}

	e->x += (float)(e->speed_x * delta);
	e->y += (float)(e->speed_y * delta);
	e->is_on_floor = 0;

	if (e->status != NORMAL) {
		e->timer -= delta;
		if (e->timer <= 0) {
			e->status = NORMAL;
		}
	}
	/*altre azioni*/
	if (e->type_in_hand == OTHER) {
		do_other_update(e, delta);
	}
	if (e->status != ATTACKING && e->status != STUNNED && button_state(B_PUNCH, e->player_id)) {//ho usato state perchè tanto ho l'as, e non sarebbe bello spaccare il mouse se è alto
		attack(world, e);
	}
	if (e->status != STUNNED && button_pressed(B_PICKUP, e->player_id)) {
		player_interact (e, world);
	}
	/*testo i pickupable*/
	if (button_pressed(B_EMOTE1, e->player_id)) {
		spawn_random_pickupable(e->x,e->y+1,world);
	}
	e->animation.animation_time += (float) delta * 1000.f;
}

void player_render(Entity *e) {
	e->animation.flip = !e->is_facing_right;
	draw(&game.renderer, &e->animation, e->x, e->y);

	const ObjectDrawInfo *di = get_object_draw_info(e->type_in_hand, e->id_in_hand);
	if (di != NULL && di->texture != NULL) {
		draw(&game.renderer, load_texture(di->texture), e->x+(e->width - di->size.x)/2, e->y+(e->height - di->size.y)/2, di->size.x, di->size.y, 0, 0, 1, 1);
	}
}

void wall_render(Entity *e) {
	draw(&game.renderer, e->texture, e->x, e->y, e->width, e->height, 0, 0, 1, 1);
}



const render_func render_functions[_RENDER_FUNCTION_COUNT] = {//funzioni
	player_render, 
	wall_render
};

const char *render_func_names[_RENDER_FUNCTION_COUNT] = {//stringhe corrispondintenti (per lettura da file)
	"RENDER_PLAYER",
	"RENDER_WALL"
};

const update_func update_functions[_UPDATE_FUNCTION_COUNT] = {
	player_update
};

const char *update_func_names[_UPDATE_FUNCTION_COUNT] = {
	"UPDATE_PLAYER"
};

void int_set_init(IntSet *set, int initial_size) {
	set -> count = 0;
	set -> size = initial_size;
	set -> elements = (int *) malloc(sizeof(int)*initial_size);
}
int int_set_add(IntSet *set, int element) {
	if (set->count >= set->size) {
		set -> size = set->count*2 + 1;
		set -> elements = (int *) realloc(set -> elements, sizeof(int)*set->size );
	}
	if (int_set_contains(set, element)) {
		return 0;
	}
	set->elements[set->count++] = element;
	return 1;
}
int int_set_contains(IntSet *set, int element) {
	for (int i = 0; i < set->count; i++) {
		if (set->elements[i] == element) {
			return 1;
		}
	}
	return 0;
}
void int_set_destroy(IntSet *set) {
	free(set -> elements);
}