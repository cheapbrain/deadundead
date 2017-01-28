#include <stdlib.h>
#include "gameplay.h"
#include "objects_data.h"
#include "utils.h"
#include "screen.h"
#include "physics.h"


void damage_entity(Entity *e, float damage) {
	if (e->status != INVULNERABLE) {
		e->health -= damage;
	}
}

void stun_entity(Entity *e, double timer) {
	if (e->status != INVULNERABLE) {
		e->status = STUNNED;
		e->timer = timer;
	}
}

/*TestHitbox*/
static void update_hitbox (Entity *e, World *world, double delta) {
	e->timer -= delta;
	if (e->timer <= 0) {
		world_remove_entity(world, e->index);
	}
}
void test_hitbox(World *world, Rectangle *hitbox) {
	Entity *e = world_new_entity(world, RENDER | UPDATE);
	e->timer = 0.5;
	e->x = hitbox->pos.x;
	e->y = hitbox->pos.y;
	e->width = hitbox->size.x;
	e->height = hitbox->size.y;
	e->texture = load_texture("../images/hitbox.png");
	e->update = &update_hitbox;
	e->render = &wall_render;
}


/***************ATTACCHI*****************/
static void melee_attack(World *world, Entity *source) {
	//TODO nell'update andrà cosa succede quando colpito?
	EntityList *hl = &(world->lists[HITTABLE_LIST]);
	const WeaponDataMelee *wd = get_data_melee(source->id_in_hand);

	Rectangle hit_zone = wd->hitbox;
	hit_zone.pos.y += source->y;
	if (source->is_facing_right) {
		hit_zone.pos.x += source->x;
	} else {
		hit_zone.pos.x = source->x + source->width - hit_zone.size.x - hit_zone.pos.x;
	}
	test_hitbox(world, &hit_zone);
	for (int i = 0; i < hl->count; i++) {
		Entity *e = world->entities + hl->entity_indexes[i];
		Rectangle e_hitbox = {{e->x, e->y},{e->width, e->height}};
		if (e != source && collides(&hit_zone, &e_hitbox)) {
			damage_entity(e, wd->damage);
		}
	}
	source->status = ATTACKING;
	source->timer = wd->attack_timer;
};

static void projectile_attack(World *world, Entity *source) {
	create_projectile(world, source, source->id_in_hand);
	source->id_in_hand = 0;
	source->type_in_hand = MELEE;
}

static void shooter_attack(World *world, Entity *source) {
	const WeaponDataShooter *data = get_data_shooter(source->id_in_hand);
	create_projectile(world, source, data->projectile_id);
	source->status = ATTACKING;
	source->timer = data->attack_timer;
}

void attack(World *world, Entity *source) {
	switch (source->type_in_hand) {
	case MELEE:			melee_attack(world, source); break;
	case PROJECTILE:	projectile_attack(world, source); break;
	case SHOOTER:		shooter_attack(world, source); break;
	case OTHER:			do_other_attack(world, source); break;
	}
}

/**********INTERAZIONE*************/
void player_interact (Entity *player, World *world) {
	Vec2 player_center;
	Entity *interacted;
	
	player_center.x = player->x + player->width/2;
	player_center.y = player->y + player->height/2;
#define PLAYER_MAXIMUM_INTERACT_DISTANCE 1 /*TODO*/
	interacted = get_closest(&player_center, PLAYER_MAXIMUM_INTERACT_DISTANCE, world, ACTIVE_EVENT_LIST);
#undef PLAYER_MAXIMUM_INTERACT_DISTANCE
	if (interacted != NULL) {
		interacted->on_interact(player, interacted, world);
	}
}