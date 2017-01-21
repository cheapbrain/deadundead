#include "gameplay.h"

PLAYER_MAXIMUM_INTERACT_DISTANCE //TODO

const WeaponDataMelee wd_melee[] = { //indice<->id_in_hand (con type_in_hand=melee)

};

static void melee_attack(World *world, Entity *source) {
	//TODO nell'update andrà cosa succede quando la salute raggiunge 0
	EntityList *hl = &(world->lists[HITTABLE_LIST]);

	Rectangle hit_zone = wd_melee[source->id_in_hand];
	hit_zone.pos.x += source->x; hit_zone.pos.y += source->y;
	for (int i = 0; i < hl->count; i++) {
		Entity *e = world_get_entity(world, hl->entity_id[i];
		Rectangle e_hitbox = {{e->x, e->y},{e->whidth, e->height}};
		if (e != source && collides(&hit_zone, &e_hitbox)) {
			e->health -= wd_melee[source->id_in_hand].damage;
		}
	}
	source->status = ATTACKING;
	source->timer = wd_melee[source->id_in_hand].attack_timer;
};


void player_interact (Entity *player, World *world) {
	Vec2 player_center;
	Entity *interacted;
	
	player_center.x = player->x + player->whidth/2;
	player_center.y = player->y + player->heigth/2;
	interacted = closest(&player_center, PLAYER_MAXIMUM_INTERACT_DISTANCE, world, ACTIVE_EVENT_LIST); //TODO è questa la lista?
	if (interacted != NULL) {
		interacted->on_interact(player, interacted);
	}
}

void pickupable_on_interact (Entity *source, Entity *target) {
	source->type_in_hand = target->type_in_hand;
	source->id_in_hand = target->id_in_hand;
}