#define _USE_MATH_DEFINES

#include <math.h>
#include <stdlib.h>
#include "gameplay.h"
#include "weapons_data.h"
#include "utils.h"
#include "screen.h"
#include "physics.h"

/***************ATTACCHI*****************/
static void melee_attack(World *world, Entity *source) {
	//TODO nell'update andrà cosa succede quando colpito?
	EntityList *hl = &(world->lists[HITTABLE_LIST]);
	WeaponDataMelee *wd = get_data_melee(source->id_in_hand);

	Rectangle hit_zone = wd->hitbox;
	hit_zone.pos.x += source->x; hit_zone.pos.y += source->y;
	if (!source->is_facing_right) {
		hit_zone.pos.x -= source->width;
	}
	for (int i = 0; i < hl->count; i++) {
		Entity *e = world_get_entity(world, hl->entity_id[i]);
		Rectangle e_hitbox = {{e->x, e->y},{e->width, e->height}};
		if (e != source && collides(&hit_zone, &e_hitbox)) {
			e->health -= wd->damage;
		}
	}
	source->status = ATTACKING;
	source->timer = wd->attack_timer;
};

static void projectile_update_function(Entity *self, World *world, double delta) {
	//aggiorno posizione (collisioni gestite nel world update)
	if (self->type_in_hand == 1) {	//lo uso per sapere se applicare la fisica o no
		self -> old_x = self -> x;
		self -> x += (float) ((self->speed_x)*delta);
	} else {
#define DOWNWARD_ACCELLERATION	1//TODO
#define DOWNWARD_MAX_SPEED		-1//TODO
		self -> old_x = self -> x; self -> old_y = self -> y;
		self -> x += (float) ((self->speed_x)*delta); self -> y += (float) ((self->speed_x)*delta);
		self -> speed_y -= (float) (DOWNWARD_ACCELLERATION*delta);
		if (self -> speed_y < DOWNWARD_MAX_SPEED) {
			self -> speed_y = DOWNWARD_MAX_SPEED;
		}
#undef DOWNWARD_ACCELLERATION
#undef DOWNWARD_MAX_SPEED
	}

	//controllo se attivarmi
	if (self -> timer == 0) {
		//alla collisione
		Rectangle rect_self = {{self->x, self->y},{self->width, self->height}};
		EntityList *hittable_list = &world->lists[HITTABLE_LIST];

		for (int i = 0; i < hittable_list->count; i++) {
			Entity *e = world_get_entity(world, hittable_list->entity_id[i]);
			Rectangle rect_e = {{e->x, e->y},{e->width, e->height}};
			if (e != self -> thrower && collides(&rect_self, &rect_e)) {
				self -> on_collide(self, e, world);
				break;
			}
		}
	} else {
		//alla scadenza del timer
		self -> timer -= delta;
		if (self -> timer <= 0) {
			get_data_projectile(self->id_in_hand) -> hit_func(self, NULL, world);
		}
	}
}

static void create_projectile(World *world, Entity *source, int projectile_id) {
	WeaponDataProjectile *data = get_data_projectile(projectile_id);
	Entity *p = world_new_entity(world, UPDATE | RENDER | DYNAMIC_COLLIDE);

	p->x = source->x + source->width/2; p->y = source->y + source->height/2;
	p->width = data->size.x; p->height = data->size.y;
	p->old_x = p->x; p->old_y = p->y;
	if (data->angle == 0) {
		p->speed_x = data->speed; p->speed_y = 0;
		p->type_in_hand = 1;
	} else {
		p->speed_x = (data->speed) * (float) cos(M_PI*(data->angle)/180.0);
		p->speed_y = (data->speed) * (float) sin(M_PI*(data->angle)/180.0);
		p->type_in_hand = 0;
	}
	if (!source->is_facing_right) {
		p->speed_x = -p->speed_x;
	}
	p->texture = load_texture(data->texture);
	//int is_on_floor;
	p->is_facing_right = source->is_facing_right;
	//int player_id;
	p->bounce_coeff = 0.9f;

	//PlayerStatus status;
	//double score;
	//float skill_bar;
	p->timer = data->timer;
	p->id_in_hand = projectile_id;
	//SpriterInstance *animation; //per settare quale animazione fare

	p->thrower = source;

	p->update = &projectile_update_function;
	p->render = &wall_render; //TODO
	if (data->timer == 0) {
		p->on_collide = data->hit_func;
	} else {
		p->on_collide = NULL;
	}
	p->on_hit = NULL;
	p->on_enter = NULL;
	p->on_interact = NULL;
}

static void projectile_attack(World *world, Entity *source) {
	create_projectile(world, source, source->id_in_hand);
}

static void shooter_attack(World *world, Entity *source) {
	WeaponDataShooter *data = get_data_shooter(source->id_in_hand);
	create_projectile(world, source, data->projectile_id);
	source->status = ATTACKING;
	source->timer = data->attack_timer;
}

void attack(World *world, Entity *source) {
	switch (source->type_in_hand) {
	case MELEE:			melee_attack(world, source); break;
	case PROJECTILE:	projectile_attack(world, source); break;
	case SHOOTER:		shooter_attack(world, source); break;
	default: /*TODO*/break;
	}
}

/**********INTERAZIONE*************/
void player_interact (Entity *player, World *world) {
	Vec2 player_center;
	Entity *interacted;
	
	player_center.x = player->x + player->width/2;
	player_center.y = player->y + player->height/2;
#define PLAYER_MAXIMUM_INTERACT_DISTANCE 1 /*TODO*/
	interacted = closest(&player_center, PLAYER_MAXIMUM_INTERACT_DISTANCE, world, ACTIVE_EVENT_LIST);
#undef PLAYER_MAXIMUM_INTERACT_DISTANCE
	if (interacted != NULL) {
		interacted->on_interact(player, interacted);
	}
}

void pickupable_on_interact (Entity *source, Entity *target) {
	if (source->status == ATTACKING) {
		source->timer = 0;
	}
	source->type_in_hand = target->type_in_hand;
	source->id_in_hand = target->id_in_hand;
}